using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StaticHttpServer
{
    class Program
    {
        static async Task Main(string[] args)
        {
            Console.WriteLine("Hello, world!");

            string hostUrl = "http://127.0.0.1:5500/";
            string hostDir = "view";

            for (int i = 0; i < args.Length; i++)
            {
                if (i < args.Length - 1)
                {
                    // valued argument
                    string value = args[i + 1];
                    if (args[i] == "-u")
                    {
                        hostUrl = value.EndsWith("/")
                            ? value
                            : value + "/";
                    }
                    else if (args[i] == "-d")
                    {
                        hostDir = value;
                    }
                }
            }

            StaticFileServer server = new StaticFileServer(hostUrl, hostDir);
            int res = await server.RunAsync(args);

            Console.WriteLine($"Ended with code {res}, press any key to continue...");
            Console.ReadKey();
        }
    }
}
