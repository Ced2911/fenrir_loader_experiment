use std::fs::File;
use std::path::Path;

use exoquant::*;

pub fn u8_as_u32<'a>(src: &'a [u8]) -> &'a [u32] {
    unsafe { std::slice::from_raw_parts(src.as_ptr() as *mut u32, src.len() / 4) }
}
pub fn u32_as_u8<'a>(src: &'a [u32]) -> &'a [u8] {
    unsafe { std::slice::from_raw_parts(src.as_ptr() as *mut u8, src.len() * 4) }
}
pub fn color_to_sat(color: Color) -> u32 {
    let u: u32 = 0;
    u = (0xFF << 24) | ((color.r as u32) << 16) | ((color.g as u32) << 8) | (color.r as u32);
    return u;
}

pub fn vu32_to_vu8(vu32: Vec<u32>) -> Vec<u8> {
    let vu8 = Vec::new();
    for v in vu32.iter() {
        vu8.push(((v>>24)&0xff) as u8);
        vu8.push(((v>>16)&0xff) as u8);
        vu8.push(((v>>8)&0xff) as u8);
        vu8.push(((v&0xff)) as u8);
    }
    return vu8
}

fn rgb32_to_color(rgb: u32) -> Color {
    let a = ((rgb >> 24) & 0xff) as u8;
    let r = ((rgb >> 16) & 0xff) as u8;
    let g = ((rgb >> 8) & 0xff) as u8;
    let b = ((rgb) & 0xff) as u8;

    let color = Color { a, b, g, r };

    return color;
}

pub fn image_8b_to_color(b: &[u8]) -> Vec<Color> {
    let rgb32 = u8_as_u32(b);
    let data: Vec<Color> = rgb32.iter().copied().map(rgb32_to_color).collect();
    return data;
}

pub struct Image {
    // in
    pub width: u32,
    pub height: u32,
    pub src_pixels: Vec<Color>,
    // computed
    pub palette: Vec<u8>,
    pub data: Vec<u8>,
}

impl Image {
    pub fn new(colors: Vec<Color>, width: u32, height: u32) -> Self {
        Image {
            width: width,
            height: height,
            src_pixels: colors,
            palette: Vec::new(),
            data: Vec::new(),
        }
    }

    pub fn quantization(&mut self, num_colors: usize) {
        let (palette, indexed_data) = convert_to_indexed(
            &self.src_pixels,
            self.width.try_into().unwrap(),
            num_colors,
            &optimizer::KMeans,
            &ditherer::FloydSteinberg::new(),
        );
        let palette = palette.into_iter().map(color_to_sat).collect();
        let palette_u8 = vu32_to_vu8(palette);
        self.palette.clear();
        self.data.clear();
        self.palette.copy_from_slice(&palette_u8);
        self.data.copy_from_slice(&indexed_data);
    }
}


pub fn load_png(input: &str) -> Image {
    let file = File::open(Path::new(input)).expect("Error reading");

    let decoder = png::Decoder::new(file);

    let mut image_reader = decoder.read_info().unwrap();
    let mut image_buf = vec![0; image_reader.output_buffer_size()];
    image_reader.next_frame(&mut image_buf).unwrap();

    let colors = image_8b_to_color(&image_buf);

    let img = Image::new(
        colors,
        image_reader.info().width,
        image_reader.info().height,
    );
    return img;
}
