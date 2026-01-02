using MessagingMeshCoordinator.Hubs;
using MessagingMeshCoordinator.Services;
using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.FileProviders;
using System;
using System.IO;
using MM = MessagingMeshLib.NET;

namespace MessagingMeshCoordinator
{
    public class Program
    {
        /// <summary>
        /// Main.
        /// </summary>
        static void Main(string[] args)
        {
            // We create the web application builder...
            var builder = WebApplication.CreateBuilder(args);

            // We set up MessagingMesh logging...
            MM.Logger.registerCallback((level, message) =>
            {
                Console.WriteLine($"{DateTime.UtcNow:HH:mm:ss.fff} ({level}): {message}");
            });

            // We set up SignalR...
            builder.Services.AddSignalR();

            // We set up CORS, to allow the Vite dev web UI to access our services...
            builder.Services.AddCors(options =>
            {
                options.AddDefaultPolicy(policy =>
                {
                    policy.WithOrigins("http://localhost:5173") // Vite dev server
                          .AllowAnyHeader()
                          .AllowAnyMethod()
                          .AllowCredentials();
                });
            });

            // We register the Coordinator and StatisticsManager as a singletons.
            // This lets them be used with SignalR dependency injection.
            builder.Services.AddSingleton<Coordinator>();
            builder.Services.AddSingleton(provider =>
            {
                var coordinator = provider.GetRequiredService<Coordinator>();
                return coordinator.StatisticsManager;
            });

            // We register hosted services...
            builder.Services.AddHostedService<CoordinatorHostedService>();
            builder.Services.AddHostedService<StatsBroadcasterService>();

            // We set up the app and run it, including serving static files from wwwroot...
            var app = builder.Build();

            // We enable CORS...
            app.UseCors();

            // We serve static files from wwwroot (production React build)...
            var wwwrootPath = Path.Combine(Directory.GetCurrentDirectory(), "wwwroot");
            if (Directory.Exists(wwwrootPath))
            {
                app.UseStaticFiles(new StaticFileOptions
                {
                    FileProvider = new PhysicalFileProvider(wwwrootPath),
                    RequestPath = ""
                });
            }

            // We enable routing...
            app.UseRouting();

            // We map the SignalR hub...
            app.MapHub<StatsHub>("/hubs/stats");

            // We set up the SPA fallback, to serve index.html for any unmatched routes (for React Router)...
            app.MapFallbackToFile("index.html", new StaticFileOptions
            {
                FileProvider = new PhysicalFileProvider(wwwrootPath)
            });

            app.Run();
        }
    }
}