use clap::Parser;
use serde::Serialize;
use std::convert::TryFrom;
use std::error::Error;
use std::fs::File;
use std::path::PathBuf;

mod image;

const ID_VDP2_BACKGROUND: u32 = 0 << 24;
const ID_VDP2_FOREGROUND: u32 = 1 << 24;
const ID_CRAM_BACKGROUND: u32 = 3 << 24;
const ID_CRAM_FOREGROUND: u32 = 4 << 24;

const TYPE_BITMAP: u32 = 1 << 30;
const TYPE_16_IDX_CCC: u32 = 0 << 28;
const TYPE_256_IDX_CCC: u32 = 1 << 28;
const TYPE_BITMAP_4BPP: u32 = TYPE_BITMAP | TYPE_16_IDX_CCC;

#[derive(Serialize, Debug, Copy, Clone, PartialEq)]
struct ThemeHeader {
    pub magic: u32,
    pub version: u32,
}

#[derive(Serialize, Debug, Copy, Clone, PartialEq)]
struct ThemeRessources {
    pub id: u32,
    pub param: u32,
    pub offset: u32,
    pub size: u32,
}
#[derive(Serialize)]
struct Theme {
    pub header: ThemeHeader,
    pub ressources: Vec<ThemeRessources>,
    pub data: Vec<u8>,
    #[serde(skip)]
    _cur_offset: u32,
}
impl Theme {
    fn save(&self, path: PathBuf) -> Result<(), Box<dyn Error>> {
        let mut file = File::create(path)?;

        use std::io::Write;
        let bytes: Vec<u8> = bincode::serialize(&self)?;
        return file.write_all(&bytes).map_err(|e| e.into());
    }

    fn _add_vdp_ressource(&mut self, filename: String, id_v: u32, id_c: u32) {
        // create image data
        let mut image = image::load_png(filename.as_str());
        image.quantization(16);

        // add bitmap
        let mut ressourceVdp = ThemeRessources {
            id: id_v,
            offset: self._cur_offset,
            size: 0,
            param: TYPE_BITMAP_4BPP,
        };
        ressourceVdp.size = u32::try_from(image.data.len()).unwrap();
        self._cur_offset += ressourceVdp.size;
        self.data.append(&mut image.data);

        // add cram
        let mut ressourcePal = ThemeRessources {
            id: id_c,
            offset: self._cur_offset,
            size: 0,
            param: 0,
        };
        ressourcePal.size = u32::try_from(image.palette.len()).unwrap();
        self._cur_offset += ressourcePal.size;
        self.data.append(&mut image.palette);
    }

    fn add_foreground(&mut self, filename: String) {
        self._add_vdp_ressource(filename, ID_VDP2_FOREGROUND, ID_CRAM_FOREGROUND);
    }

    fn add_background(&mut self, filename: String) {
        self._add_vdp_ressource(filename, ID_VDP2_BACKGROUND, ID_CRAM_BACKGROUND);
    }
}

#[derive(Parser, Debug)]
#[command(name ="themes", version="0.1", about, long_about = None)]
struct Cli {
    background: Option<String>,
    foreground: Option<String>,

    #[arg(short, long, value_name = "FILE")]
    output: Option<PathBuf>,
}

fn main() {
    let cli = Cli::parse();
    let mut ressources = Vec::new();
    let mut data = Vec::new();
    let mut theme = Theme {
        header: ThemeHeader {
            magic: 7777,
            version: 1,
        },
        ressources: ressources,
        data: data,
        _cur_offset: 0,
    };
    if cli.background.is_some() {
        theme.add_background(cli.background.unwrap());
    }
    if cli.foreground.is_some() {
        theme.add_foreground(cli.foreground.unwrap());
    }
    if cli.output.is_some() {
        match theme.save(cli.output.unwrap()) {
            Ok(_) => {}
            Err(_) => {}
        };
    }
    println!("Hello, world!");
}
