import express from 'express';
import IndexRoutes from './routes/index.routes';

const app = express();

// settings
app.set('port', process.env.PORT || 3000);
app.use(express.json());

// routes
app.use('/api',IndexRoutes);

export default app;