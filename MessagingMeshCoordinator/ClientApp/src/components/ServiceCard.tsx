import { Link } from 'react-router-dom';
import { Server } from 'lucide-react';
import { ServiceOverview } from '../types/stats';
import { generatePastelGradient } from '../utils/colorUtils';

interface ServiceCardProps {
  service: ServiceOverview;
}

export function ServiceCard({ service }: ServiceCardProps) {
  const formatNumber = (num: number): string => {
    if (num >= 1000000) {
      return (num / 1000000).toFixed(2) + 'M';
    } else if (num >= 1000) {
      return (num / 1000).toFixed(2) + 'K';
    }
    return num.toFixed(2);
  };

  // Generate deterministic gradient based on service name
  const headerGradient = generatePastelGradient(service.serviceName);

  return (
    <Link 
      to={`/service/${encodeURIComponent(service.serviceName)}`}
      className="service-card"
    >
      <div 
        className="service-header"
        style={{ background: headerGradient }}
      >
        <div className="service-name-container">
          <Server size={24} className="service-icon" />
          <h3 className="service-name">{service.serviceName}</h3>
        </div>
      </div>
      
      <div className="service-stats">
        <div className="stat-item">
          <div className="stat-label">Messages/sec</div>
          <div className="stat-value">{formatNumber(service.messagesPerSecond)}</div>
        </div>
        
        <div className="stat-divider"></div>
        
        <div className="stat-item">
          <div className="stat-label">Megabits/sec</div>
          <div className="stat-value">{formatNumber(service.megaBitsPerSecond)}</div>
        </div>
      </div>
    </Link>
  );
}
