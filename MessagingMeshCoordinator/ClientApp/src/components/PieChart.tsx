import { StatsPerSubject } from '../types/stats';

interface PieChartProps {
  data: StatsPerSubject[];
  dataKey: 'messagesPerSecond' | 'megaBitsPerSecond';
  colorScheme: 'blue' | 'green';
}

export function PieChart({ data, dataKey, colorScheme }: PieChartProps) {
  // Color palettes
  const colorPalettes = {
    blue: [
      '#3b82f6', '#60a5fa', '#93c5fd', '#bfdbfe', '#dbeafe',
      '#2563eb', '#1d4ed8', '#1e40af', '#1e3a8a', '#172554'
    ],
    green: [
      '#10b981', '#34d399', '#6ee7b7', '#a7f3d0', '#d1fae5',
      '#059669', '#047857', '#065f46', '#064e3b', '#022c22'
    ]
  };

  const colors = colorPalettes[colorScheme];

  // If no data, show empty state
  if (!data || data.length === 0) {
    return (
      <div className="pie-chart-empty">
        <p>No data available</p>
      </div>
    );
  }

  // Calculate total for percentages
  const total = data.reduce((sum, item) => sum + item[dataKey], 0);

  // Calculate segments
  let currentAngle = -90; // Start at top
  const segments = data.map((item, index) => {
    const value = item[dataKey];
    const percentage = (value / total) * 100;
    const angle = (value / total) * 360;
    
    const segment = {
      subject: item.subject,
      value: value,
      percentage: percentage,
      startAngle: currentAngle,
      endAngle: currentAngle + angle,
      color: colors[index % colors.length]
    };
    
    currentAngle += angle;
    return segment;
  });

  // SVG parameters
  const size = 400;
  const center = size / 2;
  const radius = size * 0.35;

  // Helper to convert polar to cartesian
  const polarToCartesian = (angle: number, r: number) => {
    const radians = (angle * Math.PI) / 180;
    return {
      x: center + r * Math.cos(radians),
      y: center + r * Math.sin(radians)
    };
  };

  // Helper to create SVG arc path
  const createArcPath = (startAngle: number, endAngle: number) => {
    const start = polarToCartesian(startAngle, radius);
    const end = polarToCartesian(endAngle, radius);
    const largeArc = endAngle - startAngle > 180 ? 1 : 0;
    
    return [
      `M ${center} ${center}`,
      `L ${start.x} ${start.y}`,
      `A ${radius} ${radius} 0 ${largeArc} 1 ${end.x} ${end.y}`,
      'Z'
    ].join(' ');
  };

  // Format numbers
  const formatValue = (value: number): string => {
    if (value >= 1000000) {
      return (value / 1000000).toFixed(2) + 'M';
    } else if (value >= 1000) {
      return (value / 1000).toFixed(2) + 'K';
    }
    return value.toFixed(2);
  };

  return (
    <div className="pie-chart">
      <svg 
        viewBox={`0 0 ${size} ${size}`}
        className="pie-chart-svg"
      >
        {segments.map((segment, index) => (
          <g key={index} className="pie-segment-group">
            <path
              d={createArcPath(segment.startAngle, segment.endAngle)}
              fill={segment.color}
              className="pie-segment"
            />
          </g>
        ))}
      </svg>

      <div className="pie-chart-legend">
        {segments.map((segment, index) => (
          <div key={index} className="legend-item">
            <div 
              className="legend-color" 
              style={{ backgroundColor: segment.color }}
            />
            <div className="legend-content">
              <div className="legend-subject">{segment.subject}</div>
              <div className="legend-stats">
                <span className="legend-value">{formatValue(segment.value)}</span>
                <span className="legend-percentage">({segment.percentage.toFixed(1)}%)</span>
              </div>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
