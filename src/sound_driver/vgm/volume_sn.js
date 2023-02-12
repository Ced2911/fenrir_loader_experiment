

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
