

function get_v(l) {
    let values = [48, 24, 12, 6, 3, 1.5, 0.8, 0.4].reverse();

    let keep = [];
    for (i = 0; i < values.length; i++) {
        if (l & (1 << i)) {
            keep.push(values[i])
        }
    }

    let sum = keep.reduce(
        (accumulator, currentValue) => accumulator + currentValue,
        0
    );

    //console.log(sum)
    return sum
}


for (let i = 0; i < 256; i++) {
    console.log(`${i} => ${get_v(i)} db`)
}

// Math.pow(2.0, -(x/16.0));
const volume_table_scsp = [
    0, 5, 11, 16, 21, 26, 32, 37, 43, 48, 53, 58, 64, 69, 74, 255
];

const volume_table = [
    32767, 26028, 20675, 16422, 13045, 10362, 8231, 6568,
    5193, 4125, 3277, 2603, 2067, 1642, 1304, 0
];

const bit_volume = [
    1 / Math.pow(2, 1 / 16),
    1 / Math.pow(2, 1 / 8),
    1 / Math.pow(2, 1 / 4),
    1 / Math.pow(2, 1 / 2),
    1 / 2,
    1 / 4,
    1 / 16,
    1 / 256
]

const vvv = []
for (i = 0; i < 256; i++) {
    let v = 0;
    if ((i & 1) == 1)
        v += bit_volume[0];
    if ((i & 2) == 2)
        v += bit_volume[1];
    if ((i & 4) == 4)
        v += bit_volume[2];
    if ((i & 8) == 8)
        v += bit_volume[3];
    if ((i & 16) == 16)
        v += bit_volume[4];
    if ((i & 32) == 32)
        v += bit_volume[5];
    if ((i & 64) == 64)
        v += bit_volume[6];
    if ((i & 128) == 128)
        v += bit_volume[7];

        console.log(i, v)
    vvv[i] = v
}

console.log(vvv)

const a = volume_table.map(v => v / 32767)
console.log(a)

const v = [
    0, // 1 => 
    (1 << 3) | (1 << 6) | (1 << 7), // 0.7943357646412549 => 0.7071067811865475 + 0.0625 + 0.00390625
    (1 << 3), // 0.630970183416242    => 0.7071067811865475
    (1 << 4),// 0.5011749626148259   => 
    (1 << 5) | (1 << 6) | (1 << 7),// 0.3981139561143834   => 0.25 + 0.0625 + 0.00390625
    (1 << 5) | (1 << 6) | (1 << 7),// 0.3162327951902829   => 0.25 + 0.0625 + 0.00390625
    (1 << 5), // 0.2511978514969329   =>  0.25
    (1 << 5), // 0.20044557023834955  => 0.25
    (1 << 5),// 0.1584826197088534   => 
    // 0.12588885158848842  => 
    // 0.1000091555528428   => 
    // 0.0794396801660207   => 
    // 0.0630817590868862   => 
    // 0.05011139255958739  => 
    // 0.03979613635670034  => 
]