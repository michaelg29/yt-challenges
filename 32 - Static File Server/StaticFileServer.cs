using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace StaticHttpServer
{
    class StaticFileServer
    {
        private HttpListener listener;

        private string hostUrl;
        private string hostDir;

        private string AbsolutePath(string route)
        {
            return (route.StartsWith("/") || hostDir.EndsWith("/"))
                ? $"{hostDir}{route}"
                : $"{hostDir}/{route}";
        }

        private string notFoundPath = "notFound.html";
        private string errorPath = "error.html";
        private string dirPath = "directory.html";

        private bool running;

        public StaticFileServer(string hostUrl, string hostDir)
        {
            this.hostUrl = hostUrl;
            this.hostDir = hostDir;
        }

        private async Task ProcessRequest(HttpListenerContext ctx)
        {
            string route = ctx.Request.Url.AbsolutePath;
            if (route == "/shutdown")
            {
                running = false;
                ctx.Response.StatusCode = (int)HttpStatusCode.NoContent;
                return;
            }

            string absolutePath = AbsolutePath(route);

            if (File.Exists(absolutePath))
            {
                await WriteFileAsync(ctx.Response, route);
            }
            else if (Directory.Exists(absolutePath))
            {
                string directoryList = string.Empty;
                string fileList = string.Empty;

                if (!route.EndsWith("/"))
                {
                    route += "/";
                }

                directoryList = string.Join("", Directory.GetDirectories(absolutePath)
                    .Select(s =>
                    {
                        int idx = Math.Max(s.LastIndexOf('/'), s.LastIndexOf('\\'));
                        string relPath = s.Substring(idx + 1);
                        return $"\n\t\t<ul><a href=\"{route}{relPath}\">{relPath}</a></ul>";
                    }));
                if (route.Length > 1)
                {
                    int n = route.Length - 2;
                    int idx = Math.Max(route.LastIndexOf('/', n), route.LastIndexOf('\\', n));
                    string parentPath = route.Substring(0, idx + 1);
                    directoryList += $"\n\t\t<ul><a href=\"{parentPath}\">..</a></ul>";
                }

                fileList = string.Join("", Directory.GetFiles(absolutePath)
                    .Select(s =>
                    {
                        int idx = Math.Max(s.LastIndexOf('/'), s.LastIndexOf('\\'));
                        string relPath = s.Substring(idx + 1);
                        return $"\n\t\t<ul><a href=\"{route}{relPath}\">{relPath}</a></ul>";
                    }));

                await WriteFileAsync(ctx.Response, dirPath,
                    route, directoryList, fileList);
            }
            else
            {
                ctx.Response.StatusCode = (int)HttpStatusCode.NotFound;
                await WriteFileAsync(ctx.Response, notFoundPath);
            }
        }

        public async Task WriteFileAsync(HttpListenerResponse response, string filePath, params string[] args)
        {
            try
            {
                if (args.Count() > 0)
                {
                    await SendFileFormattedAsync(response, AbsolutePath(filePath), args);
                }
                else
                {
                    await SendFileAsync(response, AbsolutePath(filePath));
                }
            }
            catch (HttpListenerException e)
            {
                response.StatusCode = (int)HttpStatusCode.InternalServerError;
                Console.WriteLine($"HttpException: {e.Message}");
            }
            catch (Exception e)
            {
                response.StatusCode = (int)HttpStatusCode.InternalServerError;
                Console.WriteLine($"{e.GetType()}: {e.Message}");
                await SendFileFormattedAsync(
                    response, AbsolutePath(errorPath),
                    e.GetType().ToString(), e.Message);
            }
        }

        private async Task SendFileAsync(HttpListenerResponse response, string filePath)
        {
            await response.OutputStream.FlushAsync();
            Stream input = new FileStream(filePath, FileMode.Open);

            try
            {
                response.ContentLength64 = input.Length;
                response.ContentEncoding = Encoding.UTF8;
                response.ContentType = Mime.GetMimeType(filePath);

                byte[] buffer = new byte[65536];
                int noBytes;
                while ((noBytes = input.Read(buffer, 0, buffer.Length)) > 0)
                {
                    await response.OutputStream.WriteAsync(buffer, 0, noBytes);
                    await response.OutputStream.FlushAsync();
                }
                input.Close();
            }
            catch
            {
                input.Close();
                throw;
            }
        }

        private async Task SendFileFormattedAsync(HttpListenerResponse response, string filePath, params string[] args)
        {
            await response.OutputStream.FlushAsync();

            string content = File.ReadAllText(filePath);
            try
            {
                string formatted = string.Format(content, args);
                content = formatted;
            }
            catch (Exception e)
            {
                Console.WriteLine($"Could not format file: {e.Message}");
            }

            response.ContentLength64 = content.Length;
            response.ContentEncoding = Encoding.UTF8;
            response.ContentType = Mime.GetMimeType(filePath);

            byte[] buffer = Encoding.UTF8.GetBytes(content);
            await response.OutputStream.WriteAsync(buffer, 0, content.Length);
            await response.OutputStream.FlushAsync();
        }

        public async Task<int> RunAsync(string[] args)
        {
            if (running)
            {
                return 1;
            }

            try
            {
                listener = new HttpListener();
                listener.Prefixes.Add(hostUrl);
                listener.Start();
            }
            catch (Exception e)
            {
                Console.WriteLine($"Could not start server: {e.Message}");
                return -1;
            }
            Console.WriteLine($"Listening on {hostUrl}, content from {hostDir}");
            running = true;

            while (running)
            {
                Task<HttpListenerContext> ctxTask = listener.GetContextAsync();
                HttpListenerContext ctx = await ctxTask;

                Console.WriteLine($"New request: {ctx.Request.HttpMethod} {ctx.Request.Url.AbsolutePath}");

                await ProcessRequest(ctx);

                Console.WriteLine($"Responding: {ctx.Response.StatusCode}, {ctx.Response.ContentType}, {ctx.Response.ContentLength64}");

                ctx.Response.Close();
            }

            listener.Close();

            return 0;
        }
    }
}
