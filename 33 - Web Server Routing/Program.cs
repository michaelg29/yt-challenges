using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using static System.Net.Http.HttpMethod;

namespace WebServerRouting
{
    class TestClass
    {
        public string name { get; set; }
        public int age { get; set; }
        public int grade { get; set; }
        public string home { get; set; }
    }

    class Program
    {
        static void Action_()
        {
            Console.WriteLine("Action_");
        }

        static void Action_Hello()
        {
            Console.WriteLine("Action_Hello");
        }

        static void Action_int(int num)
        {
            Console.WriteLine($"Action_int: {num}");
        }

        static void Action_hello_test_int_str(int num, string str)
        {
            Console.WriteLine($"Action_hello_test_int_str: {num}, {str}");
        }

        static void Action_hello_query(string name, int age)
        {
            Console.WriteLine($"Hello, {name}, with age {age}");
        }

        static void Action_body(TestClass testClass)
        {
            Console.WriteLine($"Hello, {testClass.name} ({testClass.age}), welcome to grade {testClass.grade}, I hope the journey from {testClass.home} wasn't too difficult.");
        }

        static void Main(string[] args)
        {
            //object val;
            //Type type;
            //string[] vals =
            //{
            //    "15",
            //    "57.5",
            //    "true",
            //    "a;sdgau35hi4321u",
            //    Guid.NewGuid().ToString()
            //};
            //foreach (string s in vals)
            //{
            //    if (ArgType.TryParse(s, out val, out type))
            //    {
            //        Console.WriteLine($"{s} => {type.Name}");
            //    }
            //    else
            //    {
            //        Console.WriteLine($"Could not parse {s}");
            //    }
            //}

            RouteTree rt = new RouteTree();
            rt.AddRoute(Get, "/", Action_);
            rt.AddRoute(Post, "/hello", Action_Hello);
            rt.AddRoute<int>(Put, "/{num:int}", Action_int);
            rt.AddRoute<int, string>(Patch, "/hello/test/{num:int}/{str}", Action_hello_test_int_str);
            rt.AddRoute<string, int>(Get, "/hello/query", Action_hello_query);
            rt.AddRoute<TestClass>(Put, "/greet", Action_body);

            TestClass testClass = new TestClass
            {
                name = "Michael",
                age = 124,
                grade = 123,
                home = "Antarctica"
            };

            Console.WriteLine(rt.TryNavigate(Get, "/"));
            Console.WriteLine(rt.TryNavigate(Post, "/hello"));
            Console.WriteLine(rt.TryNavigate(Put, "/15"));
            Console.WriteLine(rt.TryNavigate(Patch, "/hello"));
            Console.WriteLine(rt.TryNavigate(Delete, "/15"));
            Console.WriteLine(rt.TryNavigate(Patch, "/hello/test/17/asdg"));
            Console.WriteLine(rt.TryNavigate(Get, "/hello/query?age=123"));
            Console.WriteLine(rt.TryNavigate(Put, "/greet", 
                JsonSerializer.Serialize<TestClass>(testClass)));

            //List<int> nums = new List<int>();
            //for (int i = 0; i < 16; i++)
            //{
            //    nums.Add(i);
            //    string str = $@"        public void AddRoute<{string.Join(", ", nums.Select(n => $"T{n}"))}>
            //(HttpMethod method, string route, Action<{string.Join(", ", nums.Select(n => $"T{n}"))}> function)
            //    => _AddRoute(method, route, function);";

            //    Console.WriteLine(str + Environment.NewLine);
            //}
        }
    }
}
