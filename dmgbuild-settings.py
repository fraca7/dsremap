#!/usr/bin/env python3

format = 'UDBZ'
files = ['dist/dsremap.app']
symlinks = { 'Applications': '/Applications' }
icon_locations = {
    'dsremap': (140, 120),
    'Applications': (500, 0),
    }
window_rect = ((100, 100), (640, 280))
background = 'builtin-arrow'
default_view = 'icon-view'
grid_offset = (0, 0)
grid_spacing = 100
scroll_position = (0, 0)
label_pos = 'bottom'
text_size = 16
icon_size = 128
