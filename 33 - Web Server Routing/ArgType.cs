using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WebServerRouting
{
    public class ArgType
    {
        private static IList<ArgType> argTypes = new List<ArgType>
        {
            new ArgType(typeof(string), s => s, "str"),
            new ArgType(typeof(Guid), s => Guid.Parse(s)),
            new ArgType(typeof(double), s => double.Parse(s), "float"),
            new ArgType(typeof(int), s => int.Parse(s), "int"),
            new ArgType(typeof(bool), s => bool.Parse(s), "bool")
        };

        public static void RegisterType<T>(Func<string, object> parser, params string[] altNames)
        {
            ArgType existingArgType = argTypes.Where(at => at.Type == typeof(T)).SingleOrDefault();
            if (existingArgType != null)
            {
                existingArgType.Parser = parser;
                existingArgType.AltNames.Concat(altNames);
            }
            else
            {
                argTypes.Add(new ArgType(typeof(T), parser, altNames));
            }
        }

        public static bool TryParse(string valStr, out object val, out Type type)
        {
            // priority at top of list
            for (int i = argTypes.Count - 1; i >= 0; i--)
            {
                var argType = argTypes[i];
                try
                {
                    val = argType.Parser(valStr);
                    type = argType.Type;
                    return true;
                }
                catch
                {
                    continue;
                }
            }

            val = null;
            type = null;
            return false;
        }

        public static bool TryParse<T>(string valStr, out T val)
        {
            try
            {
                val = (T)GetArgType(typeof(T)).Parser(valStr);
                return true;
            }
            catch
            {
                val = default;
                return false;
            }
        }

        public static ArgType GetArgType(Type argType)
        {
            return argTypes
                .Where(a => a.Type == argType)
                .FirstOrDefault();
        }

        public static ArgType GetArgType(string argTypeStr)
        {
            argTypeStr = argTypeStr.ToLower();
            return argTypes
                .Where(a => a.AltNames.Contains(argTypeStr))
                .FirstOrDefault();
        }

        public Type Type { get; private set; }
        public Func<string, object> Parser { get; private set; }
        public HashSet<string> AltNames { get; private set; }

        private ArgType(Type type, Func<string, object> parser, params string[] altNames)
        {
            Type = type;
            Parser = parser;

            AltNames = new HashSet<string>();
            foreach (string altName in altNames)
            {
                AltNames.Add(altName.ToLower());
            }
            AltNames.Add(type.Name.ToLower());
        }
    }
}
