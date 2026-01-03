import { useState, useEffect, useMemo } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { ArrowLeft, Play, Pause, Server } from 'lucide-react';
import { PieChart, Pie, Cell, ResponsiveContainer } from 'recharts';
import { signalRService } from '../services/signalRService';
import { ServiceDetails, StatsPerSubject } from '../types/stats';
import { SubjectStatsTable } from '../components/SubjectStatsTable';
import { ConnectionStatus } from '../components/ConnectionStatus';
import { HubConnectionState } from '@microsoft/signalr';
import { generatePastelColor } from '../utils/colorUtils';

interface ChartDataItem {
  subject: string;
  value: number;
  percentage: number;
  color: string;
  [key: string]: string | number; // Index signature for Recharts compatibility
}

export function ServiceDetail() {
  const { serviceName } = useParams<{ serviceName: string }>();
  const navigate = useNavigate();
  const [serviceDetails, setServiceDetails] = useState<ServiceDetails | null>(null);
  const [isPaused, setIsPaused] = useState(false);
  const [connectionState, setConnectionState] = useState<HubConnectionState | null>(null);
  const [lastUpdate, setLastUpdate] = useState<Date | null>(null);

  useEffect(() => {
    if (!serviceName) {
      navigate('/');
      return;
    }

    // Set the page title
    document.title = `Messaging Mesh: ${serviceName}`;

    let isSubscribed = false;

    const setupSubscription = async () => {
      try {
        // Ensure SignalR is connected
        await signalRService.start();

        // Subscribe to service details
        await signalRService.subscribeToServiceDetails(serviceName);
        isSubscribed = true;

        // Listen for service details updates
        const unsubscribe = signalRService.onServiceDetailsUpdate(serviceName, (data) => {
          if (!isPaused) {
            setServiceDetails(data);
            setLastUpdate(new Date());
          }
        });

        // Poll connection state
        const stateInterval = setInterval(() => {
          setConnectionState(signalRService.getConnectionState());
        }, 500);

        // Cleanup
        return () => {
          unsubscribe();
          clearInterval(stateInterval);
          if (isSubscribed) {
            signalRService.unsubscribeFromServiceDetails(serviceName);
          }
        };
      } catch (err) {
        console.error('Failed to subscribe to service details:', err);
      }
    };

    const cleanup = setupSubscription();

    return () => {
      cleanup.then(fn => fn && fn());
    };
  }, [serviceName, navigate, isPaused]);

  const formatNumber = (num: number): string => {
    if (num >= 1000000) {
      return (num / 1000000).toFixed(2) + 'M';
    } else if (num >= 1000) {
      return (num / 1000).toFixed(2) + 'K';
    }
    return num.toFixed(2);
  };

  // Process chart data with 'Other' calculation
  const processChartData = (
    subjects: StatsPerSubject[],
    total: number,
    dataKey: 'messagesPerSecond' | 'megaBitsPerSecond'
  ): { chartData: ChartDataItem[]; tableData: Array<StatsPerSubject & { percentage: number; color: string }> } => {
    // Calculate sum of top subjects
    const sumTopSubjects = subjects.reduce((sum, item) => sum + item[dataKey], 0);
    const otherValue = Math.max(0, total - sumTopSubjects);

    // Create chart data with colors
    const chartData: ChartDataItem[] = subjects.map((item) => ({
      subject: item.subject,
      value: item[dataKey],
      percentage: total > 0 ? (item[dataKey] / total) * 100 : 0,
      color: generatePastelColor(item.subject, 70, 65),
    }));

    // Add 'Other' if it exists
    if (otherValue > 0.01) {
      chartData.push({
        subject: 'Other',
        value: otherValue,
        percentage: (otherValue / total) * 100,
        color: generatePastelColor('Other', 70, 65),
      });
    }

    // Create table data (includes all fields for the table)
    const tableData = subjects.map((item, index) => ({
      ...item,
      percentage: total > 0 ? (item[dataKey] / total) * 100 : 0,
      color: chartData[index].color,
    }));

    // Add 'Other' to table data
    if (otherValue > 0.01) {
      tableData.push({
        subject: 'Other',
        messagesProcessed: 0,
        bytesProcessed: 0,
        messagesPerSecond: dataKey === 'messagesPerSecond' ? otherValue : 0,
        megaBitsPerSecond: dataKey === 'megaBitsPerSecond' ? otherValue : 0,
        percentage: (otherValue / total) * 100,
        color: chartData[chartData.length - 1].color,
      });
    }

    return { chartData, tableData };
  };

  const messagesChartData = useMemo(() => {
    if (!serviceDetails) return null;
    return processChartData(
      serviceDetails.topSubjects_MessagesPerSecond,
      serviceDetails.totalMessagesPerSecond,
      'messagesPerSecond'
    );
  }, [serviceDetails]);

  const megabitsChartData = useMemo(() => {
    if (!serviceDetails) return null;
    return processChartData(
      serviceDetails.topSubjects_MegaBitsPerSecond,
      serviceDetails.totalMegaBitsPerSecond,
      'megaBitsPerSecond'
    );
  }, [serviceDetails]);

  const handleBackClick = () => {
    navigate('/');
  };

  const handleTogglePause = () => {
    setIsPaused(!isPaused);
  };

  // Custom label for pie chart slices
  const renderCustomLabel = (props: any) => {
    const { cx, cy, midAngle, outerRadius, percent, subject } = props;
    
    // Only show label if percentage is large enough (3%)
    if (percent < 0.03) return null;

    const RADIAN = Math.PI / 180;
    const radius = outerRadius + 30;
    const x = cx + radius * Math.cos(-midAngle * RADIAN);
    const y = cy + radius * Math.sin(-midAngle * RADIAN);

    return (
      <text
        x={x}
        y={y}
        fill="var(--text-primary)"
        textAnchor={x > cx ? 'start' : 'end'}
        dominantBaseline="central"
        fontSize="13px"
        fontWeight="500"
      >
        {subject}
      </text>
    );
  };

  // Custom connector line component
  const CustomLabelLine = (props: any) => {
    const { cx, cy, midAngle, outerRadius, percent } = props;
    
    // Only show connector if percentage is large enough (3%)
    if (percent < 0.03) return null;

    const RADIAN = Math.PI / 180;
    const innerRadius = outerRadius + 5;
    const outerLineRadius = outerRadius + 25;
    
    const x1 = cx + innerRadius * Math.cos(-midAngle * RADIAN);
    const y1 = cy + innerRadius * Math.sin(-midAngle * RADIAN);
    const x2 = cx + outerLineRadius * Math.cos(-midAngle * RADIAN);
    const y2 = cy + outerLineRadius * Math.sin(-midAngle * RADIAN);

    return (
      <line
        x1={x1}
        y1={y1}
        x2={x2}
        y2={y2}
        stroke="var(--border-hover)"
        strokeWidth={1}
      />
    );
  };

  if (!serviceName) {
    return null;
  }

  return (
    <div className="service-detail">
      <header className="detail-header">
        <div className="header-content">
          <div className="header-left">
            <button className="back-button" onClick={handleBackClick}>
              <ArrowLeft size={20} />
              <span>Back to Services</span>
            </button>
            <div className="service-title-section">
              <Server size={28} className="detail-icon" />
              <h1 className="service-title">{serviceName}</h1>
            </div>
          </div>
          <div className="header-right">
            <button 
              className={`pause-button ${isPaused ? 'paused' : ''}`}
              onClick={handleTogglePause}
              title={isPaused ? 'Resume updates' : 'Pause updates'}
            >
              {isPaused ? <Play size={18} /> : <Pause size={18} />}
              <span>{isPaused ? 'Resume' : 'Pause'}</span>
            </button>
            <ConnectionStatus state={connectionState} />
            {lastUpdate && (
              <span className="last-update">
                Last update: {lastUpdate.toLocaleTimeString()}
              </span>
            )}
          </div>
        </div>
      </header>

      <main className="detail-main">
        {!serviceDetails ? (
          <div className="loading-state">
            <div className="loading-spinner"></div>
            <p>Loading service details...</p>
          </div>
        ) : (
          <div className="charts-grid">
            {/* Messages per Second Chart */}
            <div className="chart-card">
              <div className="chart-header">
                <h2>Top Subjects by Messages/sec</h2>
                <div className="chart-total">
                  Total: <span className="total-value">{formatNumber(serviceDetails.totalMessagesPerSecond)}</span> msg/sec
                </div>
              </div>
              
              {messagesChartData && messagesChartData.chartData.length > 0 ? (
                <>
                  <div className="chart-container-large">
                    <ResponsiveContainer width="100%" height={400}>
                      <PieChart>
                        <Pie
                          data={messagesChartData.chartData}
                          cx="50%"
                          cy="50%"
                          outerRadius={120}
                          dataKey="value"
                          label={renderCustomLabel}
                          labelLine={<CustomLabelLine />}
                        >
                          {messagesChartData.chartData.map((entry, index) => (
                            <Cell key={`cell-${index}`} fill={entry.color} />
                          ))}
                        </Pie>
                      </PieChart>
                    </ResponsiveContainer>
                  </div>

                  <div className="table-section">
                    <h3 className="table-title">Subject Details</h3>
                    <SubjectStatsTable 
                      data={messagesChartData.tableData} 
                      dataKey="messagesPerSecond"
                    />
                  </div>
                </>
              ) : (
                <div className="no-data">No data available</div>
              )}
            </div>

            {/* Megabits per Second Chart */}
            <div className="chart-card">
              <div className="chart-header">
                <h2>Top Subjects by Megabits/sec</h2>
                <div className="chart-total">
                  Total: <span className="total-value">{formatNumber(serviceDetails.totalMegaBitsPerSecond)}</span> Mb/sec
                </div>
              </div>
              
              {megabitsChartData && megabitsChartData.chartData.length > 0 ? (
                <>
                  <div className="chart-container-large">
                    <ResponsiveContainer width="100%" height={400}>
                      <PieChart>
                        <Pie
                          data={megabitsChartData.chartData}
                          cx="50%"
                          cy="50%"
                          outerRadius={120}
                          dataKey="value"
                          label={renderCustomLabel}
                          labelLine={<CustomLabelLine />}
                        >
                          {megabitsChartData.chartData.map((entry, index) => (
                            <Cell key={`cell-${index}`} fill={entry.color} />
                          ))}
                        </Pie>
                      </PieChart>
                    </ResponsiveContainer>
                  </div>

                  <div className="table-section">
                    <h3 className="table-title">Subject Details</h3>
                    <SubjectStatsTable 
                      data={megabitsChartData.tableData} 
                      dataKey="megaBitsPerSecond"
                    />
                  </div>
                </>
              ) : (
                <div className="no-data">No data available</div>
              )}
            </div>
          </div>
        )}

        {isPaused && serviceDetails && (
          <div className="paused-banner">
            Updates paused - Click Resume to continue receiving live data
          </div>
        )}
      </main>
    </div>
  );
}
