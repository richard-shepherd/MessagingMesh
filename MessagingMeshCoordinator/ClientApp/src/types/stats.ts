// Types matching the C# DTOs from MessagingMeshCoordinator

export interface ServiceOverview {
  serviceName: string;
  messagesPerSecond: number;
  megaBitsPerSecond: number;
}

export interface StatsPerSubject {
  subject: string;
  messagesProcessed: number;
  bytesProcessed: number;
  messagesPerSecond: number;
  megaBitsPerSecond: number;
}

export interface StatsSnapshot {
  serviceName: string;
  gatewayName: string;
  durationSeconds: number;
  total: StatsPerSubject;
  topSubjects_MessagesPerSecond: StatsPerSubject[];
  topSubjects_MegaBitsPerSecond: StatsPerSubject[];
}

export interface ServiceDetails {
  serviceName: string;
  totalMessagesPerSecond: number;
  totalMegaBitsPerSecond: number;
  topSubjects_MessagesPerSecond: StatsPerSubject[];
  topSubjects_MegaBitsPerSecond: StatsPerSubject[];
}
