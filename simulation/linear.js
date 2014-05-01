function test(){
var body = {
	v: 0,
	x: 0,
	m: 3,
	acc: function(f, dt){
		// f = ma
		with(this){
			v += f * dt / m;
			if(isNaN(v)){
				console.log(f, '*', dt, '/', m);
				return false;
			}
			//console.log('velo', v);
		}

		return f;
	},
	step: function(dt){
		this.x += this.v * dt;
		console.log('X: ', this.x);
	}
};

var elapsed = 0;
var last = new Date();
var target = Math.random() * 100 - 50;

var TAP = 0.75;
var TAN = 0.25;
var hundredth = 0;

// a = acc+ * p + (1-p) * acc-
function seek(A, dt){
	var dx = target - A.x, d = Math.abs(dx);
	var vi = A.v;

	var a = dx;//(Math.pow(-A.v, 3) + dx);

	A.v += a * (dt);
	if(isNaN(A.v))
		console.log('Huh');
	A.x += A.v * dt;

	if(hundredth % 100000 == 0)
	console.log('X', A.x, 'sign', (a < 0 ? '-' : '+'));

	++hundredth;
}

var force = 0, acced = false;
var timeLimit = 5, wait = 3;
console.log("Target ", target);
//console.log("Force ", force = seek(body, timeLimit), "N");

while(elapsed < timeLimit){
	var temp = new Date();
	dt = (temp - last) / 1000;

	seek(body, dt);

	if(Math.abs(body.x - target) < 0.1){
		//console.log("Done in", elapsed);
		//break;
	}

	elapsed += dt;
	last = temp;
}

console.log('Vf', body.v);
console.log('Xf', body.x);
}