import {Router} from 'express';
const router = Router();

router.get('/',(req,res)=>{
    res.json('Gracias por usarme');
});

router.post('/init',(req, res)=>{
    res.json('conexion establecida...');
});

router.post('/keyPressed', (req, res)=>{
    process.stdout.write("\u001b[2J\u001b[0;0H");
    console.log(req.body);
    res.json({'key':req.body.key});
})

export default router;