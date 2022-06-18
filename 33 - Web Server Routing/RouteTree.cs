using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WebServerRouting
{
    internal static class ArrayExtensions
    {
        internal static T Get<T>(this object[] arr, int idx)
        {
            return idx < arr.Length
                ? (T)arr[idx]
                : default;
        }
    }

    public class RouteTree
    {
        private class RouteTreeNode
        {
            public ArgType ArgType { get; set; }
            public Action<object[]> ThisAction { get; set; }

            private IDictionary<string, RouteTreeNode> PlainSubRoutes { get; set; } // route => node
            private IDictionary<Type, RouteTreeNode> ArgSubRoutes { get; set; } // arg type => node

            // plain route, no action
            public RouteTreeNode() { }

            // plain route, with action
            public RouteTreeNode(Action<object[]> action)
            {
                ThisAction = action;
            }

            // arg route, with/without action
            public RouteTreeNode(ArgType argType, Action<object[]> action = null)
                : this(action)
            {
                ArgType = argType;
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

        private void _AddRoute(string route, Action<object[]> action)
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
                    ArgType argType = ArgType.GetArgType(typeStr);
                    nextNode = currentNode.TryGetArgSubRoute(argType.Type, out RouteTreeNode rtn)
                        ? rtn
                        : new RouteTreeNode(argType);
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
            currentNode.ThisAction = action;
        }

        public bool TryNavigate(string route)
        {
            if (root == null)
            {
                return false;
            }

            var currentNode = root;
            var routeArgsList = new List<object>();
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
                    routeArgsList.Add(val);
                }
                else
                {
                    return false;
                }
            }

            try
            {
                currentNode.ThisAction(routeArgsList.ToArray());
                return true;
            }
            catch
            {
                return false;
            }
        }

        #region TEMPLATE ADD METHODS
        public void AddRoute
            (string route, Action action)
                => _AddRoute(route, (obj)
                    => action());

        public void AddRoute<T0>
            (string route, Action<T0> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0)));

        public void AddRoute<T0, T1>
            (string route, Action<T0, T1> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1)));

        public void AddRoute<T0, T1, T2>
            (string route, Action<T0, T1, T2> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2)));

        public void AddRoute<T0, T1, T2, T3>
            (string route, Action<T0, T1, T2, T3> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3)));

        public void AddRoute<T0, T1, T2, T3, T4>
            (string route, Action<T0, T1, T2, T3, T4> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4)));

        public void AddRoute<T0, T1, T2, T3, T4, T5>
            (string route, Action<T0, T1, T2, T3, T4, T5> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9), obj.Get<T10>(10)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9), obj.Get<T10>(10), obj.Get<T11>(11)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9), obj.Get<T10>(10), obj.Get<T11>(11), obj.Get<T12>(12)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9), obj.Get<T10>(10), obj.Get<T11>(11), obj.Get<T12>(12), obj.Get<T13>(13)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9), obj.Get<T10>(10), obj.Get<T11>(11), obj.Get<T12>(12), obj.Get<T13>(13), obj.Get<T14>(14)));

        public void AddRoute<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>
            (string route, Action<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15> action)
                => _AddRoute(route, (obj)
                    => action(obj.Get<T0>(0), obj.Get<T1>(1), obj.Get<T2>(2), obj.Get<T3>(3), obj.Get<T4>(4), obj.Get<T5>(5), obj.Get<T6>(6), obj.Get<T7>(7), obj.Get<T8>(8), obj.Get<T9>(9), obj.Get<T10>(10), obj.Get<T11>(11), obj.Get<T12>(12), obj.Get<T13>(13), obj.Get<T14>(14), obj.Get<T15>(15)));
        #endregion
    }
}
