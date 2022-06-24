using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace WebServerRouting
{
    public class RouteTree
    {
        private class RouteTreeNode
        {
            public string ArgName { get; set; }
            public ArgType ArgType { get; set; }
            private IDictionary<HttpMethod, Delegate> Functions { get; set; }

            private IDictionary<string, RouteTreeNode> PlainSubRoutes { get; set; } // route => node
            private IDictionary<Type, RouteTreeNode> ArgSubRoutes { get; set; } // arg type => node

            // plain route
            public RouteTreeNode() { }

            // arg route
            public RouteTreeNode(string argName, ArgType argType)
            {
                ArgName = argName;
                ArgType = argType;
            }

            public Delegate this[HttpMethod key]
            {
                set
                {
                    if (Functions == null)
                    {
                        Functions = new Dictionary<HttpMethod, Delegate>();
                    }

                    Functions[key] = value;
                }
            }

            public bool TryGetFunction(HttpMethod method, out Delegate function)
            {
                if ((Functions?.ContainsKey(method)).GetValueOrDefault())
                {
                    function = Functions[method];
                    return true;
                }
                else
                {
                    function = default;
                    return false;
                }
            }

            public RouteTreeNode this[string key]
            {
                set
                {
                    if (PlainSubRoutes == null)
                    {
                        PlainSubRoutes = new Dictionary<string, RouteTreeNode>();
                    }
                    PlainSubRoutes[key] = value;
                }
            }

            public bool TryGetPlainSubRoute(string element, out RouteTreeNode rtn)
            {
                if ((PlainSubRoutes?.ContainsKey(element)).GetValueOrDefault())
                {
                    rtn = PlainSubRoutes[element];
                    return true;
                }
                else
                {
                    rtn = this;
                    return false;
                }
            }

            public RouteTreeNode this[Type key]
            {
                set
                {
                    if (ArgSubRoutes == null)
                    {
                        ArgSubRoutes = new Dictionary<Type, RouteTreeNode>();
                    }
                    ArgSubRoutes[key] = value;
                }
            }

            public bool TryGetArgSubRoute(Type argType, out RouteTreeNode rtn)
            {
                if ((ArgSubRoutes?.ContainsKey(argType)).GetValueOrDefault())
                {
                    rtn = ArgSubRoutes[argType];
                    return true;
                }
                else
                {
                    rtn = default;
                    return false;
                }
            }
        }

        private RouteTreeNode root;

        private void _AddRoute(HttpMethod method, string route, Delegate function)
        {
            if (root == null)
            {
                root = new RouteTreeNode();
            }

            var currentNode = root;
            foreach (var el in route.Split('/'))
            {
                if (string.IsNullOrEmpty(el))
                {
                    continue;
                }

                RouteTreeNode nextNode = null;
                if (el.StartsWith("{") && el.EndsWith("}"))
                {
                    string typeStr = el.Substring(1, el.Length - 2);
                    // get name
                    int idx = typeStr.IndexOf(':');
                    string name = string.Empty;
                    if (idx == -1)
                    {
                        name = typeStr;
                        typeStr = "string";
                    }
                    else
                    {
                        name = typeStr.Substring(0, idx);
                        typeStr = typeStr.Substring(idx + 1);
                    }
                    ArgType argType = ArgType.GetArgType(typeStr);
                    nextNode = currentNode.TryGetArgSubRoute(argType.Type, out RouteTreeNode rtn)
                        ? rtn
                        : new RouteTreeNode(name, argType);
                    currentNode[argType.Type] = nextNode;
                }
                else
                {
                    // plain route
                    nextNode = currentNode.TryGetPlainSubRoute(el, out RouteTreeNode rtn)
                        ? rtn
                        : new RouteTreeNode();
                    currentNode[el] = nextNode;
                }
                currentNode = nextNode;
            }
            currentNode[method] = function;
        }

        public bool TryNavigate(HttpMethod method, string route, string body = null)
        {
            if (root == null)
            {
                return false;
            }

            IDictionary<string, string> queryParams = new Dictionary<string, string>();
            int paramIdx = route.IndexOf('?');
            if (paramIdx != -1)
            {
                string[] paramArr = route.Substring(paramIdx + 1).Split('&');
                route = route.Substring(0, paramIdx);
                foreach (string paramStr in paramArr)
                {
                    string[] splitParam = paramStr.Split('=');
                    queryParams[Uri.UnescapeDataString(splitParam[0])]
                        = Uri.UnescapeDataString(splitParam[1]);
                }
            }

            var currentNode = root;
            IDictionary<string, object> routeArgs = new Dictionary<string, object>();
            foreach (var routeElement in route.Split('/'))
            {
                if (string.IsNullOrEmpty(routeElement)) continue;

                if (currentNode.TryGetPlainSubRoute(routeElement, out currentNode))
                {
                    continue;
                }
                else if (ArgType.TryParse(routeElement, out object val, out Type type)
                    && currentNode.TryGetArgSubRoute(type, out currentNode))
                {
                    routeArgs[currentNode.ArgName] = val;
                }
                else
                {
                    return false;
                }
            }

            if (currentNode.TryGetFunction(method, out var function))
            {
                bool bodyExists = !string.IsNullOrEmpty(body);
                IList<object> argsList = new List<object>();

                // construct arguments in order
                foreach (var param in function.Method.GetParameters())
                {
                    if (routeArgs.TryGetValue(param.Name, out object value))
                    {
                        // found from route
                        argsList.Add(value);
                        continue;
                    }

                    // look in query or body
                    string valueStr;
                    if (!queryParams.TryGetValue(param.Name, out valueStr))
                    {
                        // if not in query, assign as body
                        valueStr = body;
                    }

                    if (ArgType.TryParse(param.ParameterType, valueStr, out object objVal))
                    {
                        argsList.Add(objVal);
                        continue;
                    }
                        
                    try
                    {
                        argsList.Add(
                            JsonSerializer.Deserialize(valueStr, param.ParameterType));
                        continue;
                    }
                    catch { }

                    argsList.Add(null);
                }

                function.DynamicInvoke(argsList.ToArray());
                return true;
            }

            return false;
        }

        #region TEMPLATE ADD METHODS
        public void AddRoute
            (HttpMethod method, string route, Action action)
                => _AddRoute(method, route, action);

        public void AddRoute<T0>
            (HttpMethod method, string route, Action<T0> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1>
            (HttpMethod method, string route, Action<T0, T1> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2>
            (HttpMethod method, string route, Action<T0, T1, T2> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3>
            (HttpMethod method, string route, Action<T0, T1, T2, T3> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14> function)
                => _AddRoute(method, route, function);

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
            (HttpMethod method, string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> function)
                => _AddRoute(method, route, function);

        #endregion
    }
}
