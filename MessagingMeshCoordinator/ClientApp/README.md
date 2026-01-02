# MessagingMesh Coordinator Web UI

Modern React + TypeScript web interface for the MessagingMesh Coordinator with a clean, professional light theme.

## Features

- Real-time service statistics via SignalR
- Clean, modern light theme with blue accents
- Automatic reconnection handling
- Type-safe communication with backend
- Responsive design for all screen sizes

## Development Setup

### Prerequisites

- Node.js 18+ and npm
- MessagingMeshCoordinator running on http://localhost:5048

### Installation

```bash
cd MessagingMeshCoordinator/ClientApp
npm install
```

### Development Mode

Run the Vite dev server with hot module replacement:

```bash
npm run dev
```

The UI will be available at http://localhost:5173

The dev server proxies SignalR requests to the coordinator at localhost:5048.

### Production Build

Build the React app for production:

```bash
npm run build
```

This creates optimized static files in `../wwwroot/` which will be served by the ASP.NET Core application.

## Project Structure

```
ClientApp/
├── src/
│   ├── components/          # Reusable UI components
│   │   ├── ServiceCard.tsx      # Service overview card
│   │   └── ConnectionStatus.tsx # SignalR connection indicator
│   ├── pages/               # Page components
│   │   └── Dashboard.tsx        # Main dashboard page
│   ├── services/            # Business logic
│   │   └── signalRService.ts    # SignalR connection management
│   ├── types/               # TypeScript type definitions
│   │   └── stats.ts             # Stats DTOs matching C# types
│   ├── App.tsx              # Root component
│   ├── App.css              # Global styles
│   └── main.tsx             # Entry point
├── index.html               # HTML template
├── vite.config.ts           # Vite configuration
├── tsconfig.json            # TypeScript configuration
└── package.json             # Dependencies
```

## Type Safety

The TypeScript types in `src/types/stats.ts` match the C# DTOs in the coordinator:

- `ServiceOverview` → `Stats_ServiceOverview.cs`
- `StatsPerSubject` → `Stats_PerSubject.cs`
- `StatsSnapshot` → `Stats_Snapshot.cs`

This ensures compile-time checking of data flowing between backend and frontend.

## Design

The UI features a professional light theme with:
- Clean white cards on a subtle gradient background
- Blue gradient accents on headers and metrics
- Smooth hover animations
- Responsive grid layout
- Clear visual hierarchy
- Accessible color contrast

## Next Steps

- Add routing for service detail pages
- Implement detailed statistics views
- Add historical data visualization
- Add filtering and search capabilities
