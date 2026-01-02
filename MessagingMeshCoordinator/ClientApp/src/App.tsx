import { BrowserRouter, Routes, Route } from 'react-router-dom';
import { Dashboard } from './pages/Dashboard';
import { ServiceDetail } from './pages/ServiceDetail';
import './App.css';

function App() {
  return (
    <BrowserRouter>
      <div className="app">
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/service/:serviceName" element={<ServiceDetail />} />
        </Routes>
      </div>
    </BrowserRouter>
  );
}

export default App;
