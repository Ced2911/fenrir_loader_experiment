// torus
// 
//  http://paulbourke.net/geometry/torus/source1.c

const fs = require('fs');
const noise = require('./perlin.js');



let buildNoiseLut = (w, h, amplitude) => {
  let noizz = [];
  noise.seed(Math.random());

  // generate lut...
  for (var x = 0; x < w; x++) {
    noizz[x] = [];
    for (var y = 0; y < h; y++) {
      var value = noise.simplex3(x / 10, y / 10, 0);
      noizz[x][y] = value * amplitude;
    }
  }

  return noizz;
}


function formatColor(n) {
  let c = parseFloat(n.toFixed(6));
  c = (c + 1) / 2;
  c = c * 32;
  c = Math.round(c)
  if (c > 0x1F) {
    c = 0x1F;
  } else if (c < 0) {
    c = 0;
  }
  return c;
}


function formatNumber(n) {
  return n.toFixed(15);
}

function sphericalToCartesian(radius, azimuth, elevation) {
  var x = radius * Math.sin(elevation) * Math.cos(azimuth)
  var y = radius * Math.sin(elevation) * Math.sin(azimuth)
  var z = radius * Math.cos(elevation)
  return { x: x, y: y, z: z };
}

/**
 * todo...
 * Moitier d'une sphere
 */
let generatePlanet = () => {
  let stacks = 10;
  let slices = 10;
  let r = 5.0;
  let o = 1;
  let noisemap_len = 15.0;
  let quads = [];
  let normals = [];

  let content = '';

  let noiseLut = buildNoiseLut(noisemap_len * stacks, noisemap_len * slices, 1);
  //console.log(noiseLut);


  function outputVertex(quad) {
    content += (`{FIX32(${formatNumber(quad.x)}), FIX32(${formatNumber(quad.y)}), FIX32(${formatNumber(quad.z)})},\n`);
  }

  function outputColor(quad) {
    content += (`RGB16(${formatColor(quad.x)}, ${formatColor(quad.y)}, ${formatColor(quad.z)}),\n`);
  }

  function getNormals(vertex) {
    let len = Math.sqrt((vertex.x * vertex.x) + (vertex.y * vertex.y) + (vertex.z * vertex.z));
    return {
      x: vertex.x / len,
      y: vertex.y / len,
      z: vertex.z / len
    }
  }

  for (let t = 0; t < stacks; t++) // stacks are ELEVATION so they count theta
  {
    let theta1 = (t / stacks) * Math.PI;
    let theta2 = ((t + 1) / stacks) * Math.PI;

    for (let p = 0; p < slices; p++) // slices are ORANGE SLICES so the count azimuth
    {
      let phi1 = ((p) /  (o * slices)) * 2 * Math.PI; // azimuth goes around 0 .. 2*PI
      let phi2 = ((p + 1) / (o * slices)) * 2 * Math.PI;

      //phi2   phi1
      // |      |
      // 2------1 -- theta1
      // |\ _   |
      // |    \ |
      // 3------4 -- theta2
      //

      let vertex1 = sphericalToCartesian(r, phi1, theta1);
      let vertex2 = sphericalToCartesian(r, phi2, theta1);
      let vertex3 = sphericalToCartesian(r, phi1, theta2);
      let vertex4 = sphericalToCartesian(r, phi2, theta2);

      let normal1 = sphericalToCartesian(1, phi1, theta1);
      let normal2 = sphericalToCartesian(1, phi2, theta1);
      let normal3 = sphericalToCartesian(1, phi1, theta2);
      let normal4 = sphericalToCartesian(1, phi2, theta2);

      
      //vertex1 = vertex on a sphere of radius r at spherical coords theta1, phi1
      //vertex2 = vertex on a sphere of radius r at spherical coords theta1, phi2
      //vertex3 = vertex on a sphere of radius r at spherical coords theta2, phi2
      //vertex4 = vertex on a sphere of radius r at spherical coords theta2, phi1

      // facing out
      if (t == 0) { // top cap
        quads.push([
          vertex1, vertex4, vertex3, vertex1
        ]);
        normals.push([
          normal1,
          normal4,
          normal3,
          normal1,
        ]);
        //mesh -> addTri(vertex1, vertex3, vertex4); //t1p1, t2p2, t2p1
      } else if (t + 1 == stacks) { //end cap
        //mesh -> addTri(vertex3, vertex1, vertex2); //t2p2, t1p1, t1p2        
        quads.push([
          vertex1, vertex2, vertex3, vertex1
        ]);
        normals.push([
          normal1,
          normal2,
          normal3,
          normal1,
        ]);
      } else {
        // body, facing OUT:
        //mesh -> addTri(vertex1, vertex2, vertex4);
        // mesh -> addTri(vertex2, vertex3, vertex4);
        quads.push([
          //vertex1, vertex4, vertex2, vertex3
          vertex1, vertex2, vertex4, vertex3,
        ]);
        normals.push([
          normal1,
          normal2,
          normal4,
          normal3,
        ]);
      }
    }
  }

  content = (`static const Vector3D planet[${quads.length * 4}] = {\n`);


  quads.map((quad) => {
    outputVertex(quad[0]);
    outputVertex(quad[1]);
    outputVertex(quad[2]);
    outputVertex(quad[3]);
    content += (`\n`);
  });
  content += (`};\n`);

  content += (`static const Vector3D planet_normal[${normals.length * 4}] = {\n`);

  normals.map((quad) => {
    outputVertex(quad[0]);
    outputVertex(quad[1]);
    outputVertex(quad[2]);
    outputVertex(quad[3]);
  });
  content += (`};\n`);


  content += (`static const uint16_t planet_colors[${normals.length * 4}] = {\n`);

  normals.map((quad) => {
    outputColor(quad[0]);
    outputColor(quad[1]);
    outputColor(quad[2]);
    outputColor(quad[3]);
  });
  content += (`};\n`);

  content += `static const uint32_t planet_size = ${quads.length * 4};`;
  content += `static const uint32_t planet_normal_size = ${quads.length * 4};`;
  content += `static const uint32_t planet_color_size = ${quads.length * 4};`;

  
  content += 'static const model_t planet_model = {\n'
  content += '.vertices = planet,\n'
  content += '.normals = planet_normal,\n'
  content += '.colors = planet_colors,\n'
  content += `.polyCount = ${quads.length * 4},\n`
  content += '};\n';

  //process.stdout.write(content);
  fs.writeFile('../src/planet.h', content);
}

// Planete stuff.
let generateCylinder = () => {
  let slices = 24;
  let height = 0.4;
  let r = 8.0;
  let o = 1;

  let quads = [];
  let normals = [];
  let content = '';


  function getNormals(vertex) {
    let len = Math.sqrt((vertex.x * vertex.x) + (vertex.y * vertex.y) + (vertex.z * vertex.z));
    return {
      x: vertex.x / len,
      y: vertex.y / len,
      z: vertex.z / len
    }
  }


  function outputVertex(quad) {
    content += (`{FIX32(${formatNumber(quad.x)}), FIX32(${formatNumber(quad.y)}), FIX32(${formatNumber(quad.z)})},\n`);
  }

  function outputColor(quad) {
    content += (`RGB16(${formatColor(quad.x)}, ${formatColor(quad.y)}, ${formatColor(quad.z)}),\n`);
  }

  for (let t = 0; t < (slices*2); t++) {
    let theta1 = (t / (slices * o)) * Math.PI;
    let theta2 = ((t + 1) / (o * slices)) * Math.PI;
    let vertex1 = {};
    let vertex2 = {};
    let vertex3 = {};
    let vertex4 = {};
    //phi2   phi1
    // |      |
    // 1------2 -- theta1
    // |\ _   |
    // |    \ |
    // 4------3 -- theta2
    //

    vertex1.x = r * Math.cos(theta1);
    vertex2.x = r * Math.cos(theta2);
    vertex3.x = r * Math.cos(theta2);
    vertex4.x = r * Math.cos(theta1);

    vertex1.z = r * Math.sin(theta1);
    vertex2.z = r * Math.sin(theta2);
    vertex3.z = r * Math.sin(theta2);
    vertex4.z = r * Math.sin(theta1);


    vertex1.y = height;
    vertex2.y = height;
    vertex3.y = -height;
    vertex4.y = -height;

    console.log(vertex1);

    quads.push([
      vertex1, vertex2, vertex3, vertex4
    ]);
    normals.push([
      getNormals(vertex1),
      getNormals(vertex2),
      getNormals(vertex3),
      getNormals(vertex4),
    ]);
  }


  content = (`static const Vector3D cylinder[${quads.length * 4}] = {\n`);

  quads.map((quad) => {
    outputVertex(quad[0]);
    outputVertex(quad[1]);
    outputVertex(quad[2]);
    outputVertex(quad[3]);
    content+='\r\n';
  });
  content += (`};\n`);

  content += `static const uint32_t cylinder_size = ${quads.length * 4};`;

  
  content += (`static const uint16_t cylinder_colors[${normals.length * 4}] = {\n`);

  normals.map((quad) => {
    outputColor(quad[0]);
    outputColor(quad[1]);
    outputColor(quad[2]);
    outputColor(quad[3]);
  });
  content += (`};\n`);

  fs.writeFile('../src/cylinder.h', content);
}

let generateSphere = () => {
  let stacks = 5;
  let slices = 5;
  let r = 0.3;
  let noisemap_len = 15.0;
  let quads = [];
  let normals = [];

  let content = '';

  let noiseLut = buildNoiseLut(noisemap_len * stacks, noisemap_len * slices, 1);
  //console.log(noiseLut);


  function outputVertex(quad) {
    content += (`\t{FIX32(${formatNumber(quad.x)}), FIX32(${formatNumber(quad.y)}), FIX32(${formatNumber(quad.z)})},\n`);
  }

  function outputColor(quad) {
    content += (`\tRGB16(${formatColor(quad.x)}, ${formatColor(quad.y)}, ${formatColor(quad.z)}),\n`);
  }

  function getNormals(vertex) {
    let len = Math.sqrt((vertex.x * vertex.x) + (vertex.y * vertex.y) + (vertex.z * vertex.z));
    return {
      x: vertex.x / len,
      y: vertex.y / len,
      z: vertex.z / len
    }
  }

  for (let t = 0; t < stacks; t++) // stacks are ELEVATION so they count theta
  {
    let theta1 = (t / stacks) * Math.PI;
    let theta2 = ((t + 1) / stacks) * Math.PI;

    for (let p = 0; p < slices; p++) // slices are ORANGE SLICES so the count azimuth
    {
      let phi1 = ((p) / slices) * 2 * Math.PI; // azimuth goes around 0 .. 2*PI
      let phi2 = ((p + 1) / slices) * 2 * Math.PI;

      //phi2   phi1
      // |      |
      // 2------1 -- theta1
      // |\ _   |
      // |    \ |
      // 3------4 -- theta2
      //

      let vertex1 = sphericalToCartesian(r, phi1, theta1);
      let vertex2 = sphericalToCartesian(r, phi2, theta1);
      let vertex3 = sphericalToCartesian(r, phi1, theta2);
      let vertex4 = sphericalToCartesian(r, phi2, theta2);

      let normal1 = sphericalToCartesian(1, phi1, theta1);
      let normal2 = sphericalToCartesian(1, phi2, theta1);
      let normal3 = sphericalToCartesian(1, phi1, theta2);
      let normal4 = sphericalToCartesian(1, phi2, theta2);

      // Add noise...
      if (false) {
        let donoise = (vertex) => {
          let uv = { x: Math.atan2(vertex.z, vertex.x) / Math.PI / 2, y: Math.acos(vertex.y / r) / Math.PI };
          // make mirror... and make them as indx
          let nx = stacks * noisemap_len;
          let ny = slices * noisemap_len;
          uv.x = parseInt(((Math.round(uv.x * nx) % nx) + nx) % nx) + 0;
          uv.y = parseInt(((Math.round(uv.y * ny) % ny) + ny) % ny) + 0;

          let noisept = (noiseLut[uv.x][uv.y] * 2) - 1;
          noisept = 1 + (noisept * 0.08);
          vertex.x *= noisept;
          vertex.y *= noisept;
          vertex.z *= noisept;

          if (isNaN(vertex.x)) {
            //console.log(Math.atan2(bv.z,bv.x)/Math.PI/2, noiseLut[Math.round(uv.x*stacks)][Math.round(uv.y*slices)]);
            throw 'error';
          }
        }

        donoise(vertex1);
        donoise(vertex2);
        donoise(vertex3);
        donoise(vertex4);
      }


      //vertex1 = vertex on a sphere of radius r at spherical coords theta1, phi1
      //vertex2 = vertex on a sphere of radius r at spherical coords theta1, phi2
      //vertex3 = vertex on a sphere of radius r at spherical coords theta2, phi2
      //vertex4 = vertex on a sphere of radius r at spherical coords theta2, phi1

      // facing out
      if (t == 0) { // top cap
        quads.push([
          vertex1, vertex4, vertex3, vertex1
        ]);
        normals.push([
          normal1,
          normal4,
          normal3,
          normal1,
        ]);
        //mesh -> addTri(vertex1, vertex3, vertex4); //t1p1, t2p2, t2p1
      } else if (t + 1 == stacks) { //end cap
        //mesh -> addTri(vertex3, vertex1, vertex2); //t2p2, t1p1, t1p2        
        quads.push([
          vertex1, vertex2, vertex3, vertex1
        ]);
        normals.push([
          normal1,
          normal2,
          normal3,
          normal1,
        ]);
      } else {
        // body, facing OUT:
        //mesh -> addTri(vertex1, vertex2, vertex4);
        // mesh -> addTri(vertex2, vertex3, vertex4);
        quads.push([
          //vertex1, vertex4, vertex2, vertex3
          vertex1, vertex2, vertex4, vertex3,
        ]);
        normals.push([
          normal1,
          normal2,
          normal4,
          normal3,
        ]);
      }
    }
  }

  content = (`static const Vector3D uvsphere[${quads.length * 4}] = {\n`);


  quads.map((quad) => {
    outputVertex(quad[0]);
    outputVertex(quad[1]);
    outputVertex(quad[2]);
    outputVertex(quad[3]);
    content += (`\n`);
  });
  content += (`};\n`);

  content += (`static const Vector3D uvsphere_normal[${normals.length * 4}] = {\n`);

  normals.map((quad) => {
    outputVertex(quad[0]);
    outputVertex(quad[1]);
    outputVertex(quad[2]);
    outputVertex(quad[3]);
  });
  content += (`};\n`);


  content += (`static const uint16_t uvsphere_colors[${normals.length * 4}] = {\n`);

  normals.map((quad) => {
    outputColor(quad[0]);
    outputColor(quad[1]);
    outputColor(quad[2]);
    outputColor(quad[3]);
  });
  content += (`};\n`);

  content += `static const uint32_t uvsphere_size = ${quads.length * 4};\n`;
  content += `static const uint32_t uvsphere_normal_size = ${quads.length * 4};\n`;
  content += `static const uint32_t uvsphere_color_size = ${quads.length * 4};\n\n`;

  content += 'static model_t uvshere_model = {\n'
  content += '\t.vertices = uvsphere,\n'
  content += '\t.normals = uvsphere_normal,\n'
  content += '\t.colors = uvsphere_colors,\n'
  content += `.polyCount = ${quads.length * 4},\n`
  content += '};\n';

  //process.stdout.write(content);
  fs.writeFile('../src/sphere.h', content);
  /*
    // light test...
    (function () {
      let light = { x: 0.9, y: 0.9, z: -0.9 }
      let len = Math.sqrt((light.x * light.x) + (light.y * light.y) + (light.z * light.z));
      console.log({
        x: light.x / len,
        y: light.y / len,
        z: light.z / len,
      })
    })();
    */
}



let main = () => {
  generateCylinder();
  generateSphere();
  generatePlanet();
}
main();