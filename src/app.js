import express from 'express';
import IndexRoutes from './routes/index.routes';

const app = express();

app.set('port', process.env.PORT || 3000);

app.use('/api/index',IndexRoutes);