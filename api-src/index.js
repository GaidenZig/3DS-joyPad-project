import app from './app.js'

//Initializations
app.listen(app.get('port'),() =>{
    console.log("server listening on port: ",app.get('port'));
});