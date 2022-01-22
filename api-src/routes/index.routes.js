import {Router} from 'express';
const { vJoy, vJoyDevice } = require('vjoy');
const router = Router();

let joyObj={
    'KEY_A':1,
    'KEY_B':2,
    'KEY_Y':3,
    'KEY_X':4,
    'KEY_L':5,
    'KEY_R':6,
    'KEY_SELECT':7,
    'KEY_START':8,
    'KEY_DRIGHT':9,
    'KEY_DDOWN':10,
    'KEY_DLEFT':11,
    'KEY_DUP':12
}

if (!vJoy.isEnabled()) {
	console.log("vJoy is not enabled.");
	process.exit();
}

let device = vJoyDevice.create(1);
let activeButton = 0;

router.get('/',(req,res)=>{
    res.json('Gracias por usarme');
});

router.post('/init',(req, res)=>{
    console.log('se conectaron');
    res.json('conexion establecida...');
});

router.post('/keyPressed', (req, res)=>{
    process.stdout.write("\u001b[2J\u001b[0;0H");
    console.log(req.body);

    device.axes.X.set(req.body.circlePad.x); // set the X axis to some value
    device.axes.Y.set(60)
    
    // set first 12 buttons
	device.resetButtons();

    req.body.keysDown.forEach(e => {
        if(joyObj[e]){
            activeButton = joyObj[e];
            device.buttons[activeButton].set(true);
        }
    });

    req.body.keysHeld.forEach(e => {
        if(joyObj[e]){
            activeButton = joyObj[e];
            device.buttons[activeButton].set(true);
        }
    });

    req.body.keysUp.forEach(e=>{
        if(joyObj[e]){
            activeButton = joyObj[e];
            device.buttons[activeButton].set(false);
        }
    });

    activeButton = 0;

    res.json({'key':req.body.key});
})

function modClamp(value, lower, upper) {
	let diff = upper - lower;
	value -= lower;

	return lower + ((value % diff) + diff) % diff;
}

export default router;