
// toggle through lights
var sun1, sun2, moon, magic;
var mode = 0;
function daytime() {
	if (sun1)  { remove(sun1);  sun1  = undefined; }
	if (sun2)  { remove(sun2);  sun2  = undefined; }
	if (moon)  { remove(moon);  moon  = undefined; }
	if (magic) { remove(magic); magic = undefined; }
	if (mode == 0) {
		sun1 = light(0.5, 1.0, 1.5, 0, 0.75, 0.74, 0.67, 0.8, 'directional');
		sun2 = light(0.5, 1.0,-1.0, 0, 0.75, 0.74, 0.67, 0.4, 'directional');
	}
	else if (mode == 1)
		moon = light(0.5, 1.5, 1.0, 0, 0.6, 0.6, 0.8, 0.4, 'directional');
	else if (mode == 2)
		magic = light(0, 8, 0, 100, .2, .4, .9, 3.0, 'directional');
	mode = mode < 2 ? mode + 1 : 0;
}

daytime();

// helper function
function cook(from, to) { from = from || 0; to = to || 1; return (parseFloat(10000 * Math.random() % parseInt((to - from) * 10)) / 10) + from; };

// shoot cube
key(['V', 'Ctrl'], function() {
	var id = model('cube.prim', 'magic.mtl', 0, 10, 0, 0, 0, 0, 0.25, 5);
	var origin = position(camera());
	var lookat = direction(camera());
	var power = 120;
	rotation(id, 360 * cook(), 360 * cook(), 360 * cook());
	position(id, origin[0] + lookat[0], origin[1] + lookat[1], origin[2] + lookat[2]);
	impulse(id, power * lookat[0], power * lookat[1], power * lookat[2]);
});

// insert capsule
key(['C', 'Ctrl'], function () {
	var capsule = place('capsule.obj', 'capsule.mtl', 2, 1, 0, 70);
	if (!capsule) return;
	impulse(capsule, 0, 200, 0);
	print('Inserted a capsule.');
});

// place barrel
key(['B', 'Ctrl'], function () {
	var barrel = place('barrel.3ds', 'barrel.mtl', 3, 1, 0.8, 25);
	if (!barrel) return;
	rotation(barrel, -90, 0, 360 * cook());
	print('Placed a barrel.');
});

// insert rock
key(['N', 'Ctrl'], function () {
	var rock = place('rock.obj', 'rock.mtl', 4, 3, 1.5, 500);
	if (!rock) return;
	print('Inserted a rock.');
});

// set a shrine
key(['M', 'Ctrl'], function () {
	var shrine = place('shrine.3ds', 'shrine.mtl', 10, 1);
	rotation(shrine, -90, 0, 0);
	if (!shrine) return;
	print('Set a shrine.');
});

// insert texture test object
var texture = 'concrete';
key(['X', 'Ctrl'], function () {
	var cube = place('cube.prim', texture + '.mtl', 10, 2, 1, 1000);
	if (!cube) return;
	print('Inserted a textured cube.');
});

function place(mesh, material, distance, scale, shift, mass) {
	distance = distance || 2;
	scale    = scale    || 1;
	shift    = shift    || 0;
	mass     = mass     || 0;

	var origin = position(camera());
	var lookat = direction(camera());
	var target = [origin[0] + distance * lookat[0],
				  origin[1] + distance * lookat[1],
				  origin[2] + distance * lookat[2]];

	var height;
	var space = false, floor = false;
	for (var offset = 5; offset >= -5; --offset) {
		height = parseInt(origin[1] + offset);
		var solid = block(parseInt(target[0]), height, parseInt(target[2]));
		if (!space && !solid) {
			space = true;
		}
		if (space && solid) {
			floor = true;
			height++;
			break;
		}
	}
	if (!floor) {
		print('No floor for placing here.');
		return false;
	}

	return model(mesh, material, target[0], height + shift, target[2], 0, 0, 0, scale, mass);
}
