#include <yaul.h>
#include "scsp.h"
#include "ken.raw.h"
#include "midi/midi-parser.h"
#include "dbg.h"

typedef struct
{
    uint8_t type;

    midi_scsp_event_t *next;
} midi_scsp_event_t;

static void parse_and_dump(struct midi_parser *parser)
{
    enum midi_parser_status status;

    while (1)
    {
        status = midi_parse(parser);
        switch (status)
        {
        case MIDI_PARSER_EOB:
            emu_printf("eob\n");
            return;

        case MIDI_PARSER_ERROR:

            emu_printf("error\n");
            return;

        case MIDI_PARSER_INIT:
            puts("init\n");
            break;

        case MIDI_PARSER_HEADER:
            emu_printf("header\n");
            emu_printf("  size: %d\n", parser->header.size);
            emu_printf("  format: %d [%s]\n", parser->header.format, midi_file_format_name(parser->header.format));
            emu_printf("  tracks count: %d\n", parser->header.tracks_count);
            emu_printf("  time division: %d\n", parser->header.time_division);
            break;

        case MIDI_PARSER_TRACK:
            emu_printf("track");
            emu_printf("  length: %d\n", parser->track.size);
            break;

        case MIDI_PARSER_TRACK_MIDI:
            emu_printf("track-midi");
            emu_printf("  time: %ld\n", parser->vtime);
            emu_printf("  status: %d [%s]\n", parser->midi.status, midi_status_name(parser->midi.status));
            emu_printf("  channel: %d\n", parser->midi.channel);
            emu_printf("  param1: %d\n", parser->midi.param1);
            emu_printf("  param2: %d\n", parser->midi.param2);
            break;

        case MIDI_PARSER_TRACK_META:
            emu_printf("track-meta\n");
            emu_printf("  time: %ld\n", parser->vtime);
            emu_printf("  type: %d [%s]\n", parser->meta.type, midi_meta_name(parser->meta.type));
            emu_printf("  length: %d\n", parser->meta.length);
            break;

        case MIDI_PARSER_TRACK_SYSEX:
            emu_printf("track-sysex");
            emu_printf("  time: %ld\n", parser->vtime);
            break;

        default:
            emu_printf("unhandled state: %d\n", status);
            return;
        }
    }
}

void midi_init()
{
    struct midi_parser parser;
    parser.state = MIDI_PARSER_INIT;
    parser.size = sizeof(ken_midi);
    parser.in = ken_midi;

    parse_and_dump(&parser);
}