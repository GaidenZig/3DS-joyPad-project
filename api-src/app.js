import express from 'express';
import IndexRoutes from './routes/index.routes';
const exec = require('child_process').execFile;
import config from './config'

const app = express();

// settings
app.set('port', process.env.PORT || 3000);
app.use(express.json());

// routes
app.use('/api',IndexRoutes);

// middleware
var fun =function(){
   console.log("fun() start");
   exec(config.vjoy_build+'/JoyMonitor.exe', function(err, data) {  
        console.log(err)
        console.log(data.toString());                       
    });  
}
fun();

export default app;