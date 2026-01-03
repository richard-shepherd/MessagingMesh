import { useMemo, useState } from 'react';
import {
  useReactTable,
  getCoreRowModel,
  getSortedRowModel,
  flexRender,
  createColumnHelper,
  SortingState,
} from '@tanstack/react-table';
import { StatsPerSubject } from '../types/stats';

interface SubjectRow extends StatsPerSubject {
  percentage: number;
  color: string;
}

interface SubjectStatsTableProps {
  data: SubjectRow[];
  dataKey: 'messagesPerSecond' | 'megaBitsPerSecond';
}

const columnHelper = createColumnHelper<SubjectRow>();

export function SubjectStatsTable({ data, dataKey }: SubjectStatsTableProps) {
  const [sorting, setSorting] = useState<SortingState>([
    { id: dataKey, desc: true } // Default sort by the data key column, descending
  ]);

  const formatNumber = (num: number): string => {
    if (num >= 1000000) {
      return (num / 1000000).toFixed(2) + 'M';
    } else if (num >= 1000) {
      return (num / 1000).toFixed(2) + 'K';
    }
    return num.toFixed(2);
  };

  const columns = useMemo(() => [
    columnHelper.accessor('subject', {
      header: 'Subject',
      cell: (info) => (
        <div style={{ display: 'flex', alignItems: 'center', gap: '0.75rem' }}>
          <div
            style={{
              width: '12px',
              height: '12px',
              borderRadius: '3px',
              backgroundColor: info.row.original.color,
              flexShrink: 0,
            }}
          />
          <span>{info.getValue()}</span>
        </div>
      ),
    }),
    columnHelper.accessor('messagesPerSecond', {
      header: 'Messages/sec',
      cell: (info) => formatNumber(info.getValue()),
    }),
    columnHelper.accessor('megaBitsPerSecond', {
      header: 'Megabits/sec',
      cell: (info) => formatNumber(info.getValue()),
    }),
    columnHelper.accessor('percentage', {
      header: '% of Total',
      cell: (info) => `${info.getValue().toFixed(1)}%`,
    }),
  ], []);

  const table = useReactTable({
    data,
    columns,
    state: {
      sorting,
    },
    onSortingChange: setSorting,
    getCoreRowModel: getCoreRowModel(),
    getSortedRowModel: getSortedRowModel(),
  });

  return (
    <div className="stats-table-container">
      <table className="stats-table">
        <thead>
          {table.getHeaderGroups().map((headerGroup) => (
            <tr key={headerGroup.id}>
              {headerGroup.headers.map((header) => (
                <th
                  key={header.id}
                  onClick={header.column.getToggleSortingHandler()}
                  className={header.column.getCanSort() ? 'sortable' : ''}
                >
                  <div className="header-content">
                    {flexRender(
                      header.column.columnDef.header,
                      header.getContext()
                    )}
                    {header.column.getIsSorted() && (
                      <span className="sort-indicator">
                        {header.column.getIsSorted() === 'asc' ? ' ▲' : ' ▼'}
                      </span>
                    )}
                  </div>
                </th>
              ))}
            </tr>
          ))}
        </thead>
        <tbody>
          {table.getRowModel().rows.map((row) => (
            <tr key={row.id}>
              {row.getVisibleCells().map((cell) => (
                <td key={cell.id}>
                  {flexRender(cell.column.columnDef.cell, cell.getContext())}
                </td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
