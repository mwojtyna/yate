# yate

Yet Another Terminal Emulator

## Build and run

`cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j && build/yate`

## Test

`cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j && build/yate-test`

## Configuration

- Set log level with `SPDLOG_LEVEL` environment variable

## Todo

### priority

- [x] OpenGL setup
- [x] Font rendering
- [x] PTY initialization
- [ ] Terminal codes parsing
- [ ] Window resizing

### later

- [ ] Font resizing
- [ ] Bold, italic, etc.
- [ ] Emojis
- [ ] Font fallback

## Supported Terminal Sequences

Credit to https://xtermjs.org/docs/api/vtfeatures/

### C0

| Mnemonic | Name                  | Sequence | Short Description                                 | Support |
| -------- | --------------------- | -------- | ------------------------------------------------- | ------- |
| NUL      | Null                  | \0, \x00 | NUL is ignored.                                   | &cross; |
| BEL      | Bell                  | \a, \x07 | Ring the bell                                     | &cross; |
| BS       | Backspace             | \b, \x08 | Move the cursor one position to the left          | &cross; |
| HT       | Horizontal Tabulation | \t, \x09 | Move the cursor to the next character tab stop.   | &check; |
| LF       | Line Feed             | \n, \x0A | Move the cursor one row down, scrolling if needed | &check; |
| VT       | Vertical Tabulation   | \v, \x0B | Treated as LF.                                    | &cross; |
| FF       | Form Feed             | \f, \x0C | Treated as LF.                                    | &cross; |
| CR       | Carriage Return       | \r, \x0D | Move the cursor to the beginning of the row.      | &check; |
| SO       | Shift Out             | \x0E     | Switch to an alternative character set.           | &cross; |
| SI       | Shift In              | \x0F     | Return to regular character set after Shift Out.  | &cross; |
| ESC      | Escape                | \e, \x1B | Start of a sequence. Cancels any other sequence.  | &check; |

### C1

| Mnemonic | Name                        | Sequence | Short Description                                  | Support |
| -------- | --------------------------- | -------- | -------------------------------------------------- | ------- |
| IND      | Index                       | \x84     | Move the cursor one line down scrolling if needed. | &cross; |
| NEL      | Next Line                   | \x85     | Move the cursor to the beginning of the next row.  | &cross; |
| HTS      | Horizontal Tabulation Set   | \x88     | Places a tab stop at the current cursor position.  | &cross; |
| DCS      | Device Control String       | \x90     | Start of a DCS sequence.                           | &cross; |
| CSI      | Control Sequence Introducer | \x9B     | Start of a CSI sequence.                           | &check; |
| ST       | String Terminator           | \x9C     | Terminator used for string type sequences.         | &cross; |
| OSC      | Operating System Command    | \x9D     | Start of an OSC sequence.                          | &check; |
| PM       | Privacy Message             | \x9E     | Start of a privacy message.                        | &cross; |
| APC      | Application Program Command | \x9F     | Start of an APC sequence.                          | &cross; |

### CSI

| Mnemonic | Name                                  | Sequence      | Short Description                                                                  | Support |
| -------- | ------------------------------------- | ------------- | ---------------------------------------------------------------------------------- | ------- |
| ICH      | Insert Characters                     | CSI Ps @      | Insert Ps (blank) characters (default = 1)                                         | &cross; |
| SL       | Scroll Left                           | CSI Ps SP @   | Scroll viewport Ps times to the left                                               | &cross; |
| CUU      | Cursor Up                             | CSI Ps A      | Move cursor Ps times up (default=1)                                                | &cross; |
| SR       | Scroll Right                          | CSI Ps SP A   | Scroll viewport Ps times to the right                                              | &cross; |
| CUD      | Cursor Down                           | CSI Ps B      | Move cursor Ps times down (default=1)                                              | &cross; |
| CUF      | Cursor Forward                        | CSI Ps C      | Move cursor Ps times forward (default=1).                                          | &check; |
| CUB      | Cursor Backward                       | CSI Ps D      | Move cursor Ps times backward (default=1).                                         | &check; |
| CNL      | Cursor Next Line                      | CSI Ps E      | Move cursor Ps times down (default=1) and to the first column                      | &cross; |
| CPL      | Cursor Backward                       | CSI Ps F      | Move cursor Ps times up (default=1) and to the first column                        | &cross; |
| CHA      | Cursor Horizontal Absolute            | CSI Ps G      | Move cursor to Ps-th column of the active row (default=1).                         | &cross; |
| CUP      | Cursor Position                       | CSI Ps ; Ps H | Set cursor to position [Ps, Ps] (default = [1, 1])                                 | &cross; |
| CHT      | Cursor Horizontal Tabulation          | CSI Ps I      | Move cursor Ps times tabs forward (default=1).                                     | &cross; |
| ED       | Erase In Display                      | CSI Ps J      | Erase various parts of the viewport                                                | &cross; |
| DECSED   | Selective Erase In Display            | CSI ? Ps J    | Same as ED with respecting protection flag.                                        | &cross; |
| EL       | Erase In Line                         | CSI Ps K      | Erase various parts of the active row                                              | &cross; |
| DECSEL   | Selective Erase In Line               | CSI ? Ps K    | Same as EL with respecting protecting flag.                                        | &cross; |
| IL       | Insert Line                           | CSI Ps L      | Insert Ps blank lines at active row (default=1)                                    | &cross; |
| DL       | Delete Line                           | CSI Ps M      | Delete Ps lines at active row (default=1)                                          | &cross; |
| DCH      | Delete Character                      | CSI Ps P      | Delete Ps characters (default=1)                                                   | &cross; |
| SU       | Scroll Up                             | CSI Ps S      | Scroll Ps lines up (default=1).                                                    | &cross; |
| SD       | Scroll Down                           | CSI Ps T      | Scroll Ps lines down (default=1).                                                  | &cross; |
| ECH      | Erase Character                       | CSI Ps X      | Erase Ps characters from current cursor position to the right (default=1)          | &cross; |
| CBT      | Cursor Backward Tabulation            | CSI Ps Z      | Move cursor Ps tabs backward (default=1).                                          | &cross; |
| HPA      | Horizontal Position Absolute          | CSI Ps `      | Same as CHA.                                                                       | &cross; |
| HPR      | Horizontal Position Relative          | CSI Ps a      | Same as CUF.                                                                       | &cross; |
| REP      | Repeat Preceding Character            | CSI Ps b      | Repeat preceding character Ps times (default=1)                                    | &cross; |
| DA1      | Primary Device Attributes             | CSI c         | Send primary device attributes.                                                    | &cross; |
| DA2      | Secondary Device Attributes           | CSI &gt; c    | Send primary device attributes.                                                    | &cross; |
| VPA      | Vertical Position Absolute            | CSI Ps d      | Move cursor to Ps-th row (default=1).                                              | &cross; |
| VPR      | Vertical Position Relative            | CSI Ps e      | Move cursor Ps times down (default=1).                                             | &cross; |
| HVP      | Horizontal and Vertical Position      | CSI Ps ; Ps f | Same as CUP.                                                                       | &cross; |
| TBC      | Tab Clear                             | CSI Ps g      | Clear tab stops at current position (0) or all (3) (default=0)                     | &cross; |
| SM       | Set Mode                              | CSI Pm h      | Set various terminal modes                                                         | &cross; |
| DECSET   | DEC Private Set Mode                  | CSI ? Pm h    | Set various terminal attributes                                                    | &cross; |
| RM       | Reset Mode                            | CSI Pm l      | Set various terminal attributes                                                    | &cross; |
| DECRST   | DEC Private Reset Mode                | CSI ? Pm l    | Reset various terminal attributes                                                  | &cross; |
| SGR      | Select Graphic Rendition              | CSI Pm m      | Set/Reset various text attributes                                                  | &cross; |
| DSR      | Device Status Report                  | CSI Ps n      | Request cursor position (CPR) with Ps = 6.                                         | &cross; |
| DECDSR   | DEC Device Status Report              | CSI ? Ps n    | Only CPR is supported (same as DSR).                                               | &cross; |
| DECRQM   | Request Mode                          | CSI Ps $p     | Request mode state                                                                 | &cross; |
| DECSTR   | Soft Terminal Reset                   | CSI ! p       | Reset several terminal attributes to initial state                                 | &cross; |
| DECSCA   | Select Character Protection Attribute | CSI Ps " q    | Whether DECSED and DECSEL can erase (0=default, 2) or not (1).                     | &cross; |
| DECSCUSR | Set Cursor Style                      | CSI Ps SP q   | Set cursor style                                                                   | &cross; |
| DECSTBM  | Set Top and Bottom Margin             | CSI Ps ; Ps r | Set top and bottom margins of the viewport [top;bottom] (default = viewport size). | &cross; |
| SCOSC    | Save Cursor                           | CSI s         | Save cursor position, charmap and text attributes.                                 | &cross; |
| SCORC    | Restore Cursor                        | CSI u         | Restore cursor position, charmap and text attributes.                              | &cross; |
| DECIC    | Insert Columns                        | CSI Ps ' }    | Insert Ps columns at cursor position                                               | &cross; |
| DECDC    | Delete Columns                        | CSI Ps ' ~    | Delete Ps columns at cursor position                                               | &cross; |

### DCS

| Mnemonic  | Name                         | Sequence                   | Short Description                  | Support |
| --------- | ---------------------------- | -------------------------- | ---------------------------------- | ------- |
| SIXEL     | SIXEL Graphics               | DCS Ps ; Ps ; Ps ; q Pt ST | Draw SIXEL image.                  | &cross; |
| DECUDK    | User Defined Keys            | DCS Ps ; Ps \| Pt ST       | Definitions for user-defined keys. | &cross; |
| XTGETTCAP | Request Terminfo String      | DCS + q Pt ST              | Request Terminfo String.           | &cross; |
| XTSETTCAP | Set Terminfo Data            | DCS + p Pt ST              | Set Terminfo Data.                 | &cross; |
| DECRQSS   | Request Selection or Setting | DCS $ q Pt ST              | Request several terminal settings  | &cross; |

### ESC

| Mnemonic | Name                        | Sequence | Short Description                                     | Support |
| -------- | --------------------------- | -------- | ----------------------------------------------------- | ------- |
| SC       | Save Cursor                 | ESC 7    | Save cursor position, charmap and text attributes.    | &cross; |
| RC       | Restore Cursor              | ESC 8    | Restore cursor position, charmap and text attributes. | &cross; |
| DECALN   | Screen Alignment Pattern    | ESC # 8  | Fill viewport with a test pattern (E).                | &cross; |
| IND      | Index                       | ESC D    | Move the cursor one line down scrolling if needed.    | &cross; |
| NEL      | Next Line                   | ESC E    | Move the cursor to the beginning of the next row.     | &cross; |
| HTS      | Horizontal Tabulation Set   | ESC H    | Places a tab stop at the current cursor position.     | &cross; |
| IR       | Reverse Index               | ESC M    | Move the cursor one line up scrolling if needed.      | &cross; |
| DCS      | Device Control String       | ESC P    | Start of a DCS sequence.                              | &cross; |
| CSI      | Control Sequence Introducer | ESC [    | Start of a CSI sequence.                              | &cross; |
| ST       | String Terminator           | ESC \    | Terminator used for string type sequences.            | &cross; |
| OSC      | Operating System Command    | ESC ]    | Start of an OSC sequence.                             | &cross; |
| PM       | Privacy Message             | ESC ^    | Start of a privacy message.                           | &cross; |
| APC      | Application Program Command | ESC \_   | Start of an APC sequence.                             | &cross; |

### OSC

Other than listed in the table, the parser recognizes both ST (ECMA-48) and BEL (xterm) as OSC sequence finalizer.

| Identifier | Sequence                 | Short Description                                    | Support           |
| ---------- | ------------------------ | ---------------------------------------------------- | ----------------- |
| 0          | OSC 0 ; Pt BEL           | Set window title and icon name                       | Only window title |
| 1          | OSC 1 ; Pt BEL           | Set icon name.                                       | &cross;           |
| 2          | OSC 2 ; Pt BEL           | Set window title                                     | &check;           |
| 4          | OSC 4 ; c ; spec BEL     | Change color number c to the color specified by spec | &cross;           |
| 8          | OSC 8 ; params ; uri BEL | Create a hyperlink to uri using params               | &cross;           |
| 10         | OSC 10 ; Pt BEL          | Set or query default foreground color                | &cross;           |
| 11         | OSC 11 ; Pt BEL          | Same as OSC 10, but for default background.          | &cross;           |
| 12         | OSC 12 ; Pt BEL          | Same as OSC 10, but for default cursor color.        | &cross;           |
| 104        | OSC 104 ; c BEL          | Reset color number c to themed color                 | &cross;           |
| 110        | OSC 110 BEL              | Restore default foreground to themed color.          | &cross;           |
| 111        | OSC 111 BEL              | Restore default background to themed color.          | &cross;           |
| 112        | OSC 112 BEL              | Restore default cursor to themed color.              | &cross;           |
