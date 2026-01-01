using MessagingMeshCoordinator.Hubs;
using MessagingMeshCoordinator.Services;
using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using System;
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

            // We set up the app and run it...
            var app = builder.Build();
            app.UseCors();
            app.UseRouting();
            app.MapHub<StatsHub>("/hubs/stats");
            app.Run();
        }
    }
}