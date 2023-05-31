# InSCight Table Description

The following tables are used to store the tracing data. If SQL is chosen as
the database backend, all tables are stored in a single sqlite3 database called
`sim.<procid>.db`. If CSV is chosen as the database backend, each tables is
stored as its own CSV file called `<table>.<procid>.csv`.

----
## Meta
The `meta` table holds meta information about the SystemC simulation, such as
process-id, time-stamp, user-namem, etc. Only one data entry will be present.
* `pid` (`BIGINT`) OS-specific process-ID of the simulation
* `path` (`STRING`) OS-specific path to the simulation binary
* `user` (`STRING`) Username of the user who started the simulation
* `version` (`STRING`) Version string of the employed SystemC implementation
* `timestamp` (`DATIME`) Unix timestamp when the simulation was started

Example table:
| pid | path           | user  | version       | timestamp  |
|:---:|:--------------:|:-----:|:-------------:|:----------:|
|`123`|`/bin/mysim.exe`| `jan` |`SystemC 2.3.3`|`1685538009`| 

----
## Modules
The `modules` table holds information about all SystemC modules present in the
simulation. For each module, it stores the following fields:
* `id` (`BIGINT`) globally unique identifier
* `name` (`STRING`) full hierarchical name for each module
* `kind` (`STRING`) module description

Example table:
| id | name           | kind           |
|:--:|:--------------:|:--------------:|
| 12 | `system.uart0` | `vcml::pl011`  |
| 33 | `system.mem`   | `vcml::memory` |

----
## Processes
The `processes` table holds information about all SystemC processes present in
the simulation. For each process, it holds the following fields:
* `id` (`BIGINT`) globally unique identifier
* `name` (`STRING`) full hierarchcal name for each process
* `kind` (`INTEGER`) process kind (0: `SC_METHOD`, 1: `SC_THREAD`, 2: `SC_CTHREAD`)

Example table:
| id | name                | kind |
|:--:|:-------------------:| :---:|
| 49 | system.uart0.update | `0`  |
| 70 | system.cpu0.run`    | `1`  |
| 87 | system.cpu1.run`    | `1`  |

----
## Ports
The `ports` table holds information about all SystemC and TLM ports present in
the simulation. For each port, the following fields are stored:
* `id` (`BIGINT`) globally unique identifier
* `name` (`STRING`) full hierarchcal name for each port

Example table:
| id | name                   |
|:--:|:----------------------:|
| 14 | system.uart0.clk_port0 |
| 21 | system.cpu.out         |
| 25 | system.cpu.hart0.data  |

----
## Events
The `events` table holds information about all SystemC events present in the
simulation. For each event, the following fields are stored:
* `id` (`BIGINT`) globally unique identifier
* `name` (`STRING`) full hierarchcal name for each event

Example table:
| id | name                    |
|:--:|:-----------------------:|
| 26 | system.loader.clkrst_ev |
| 80 | system.bus.event_0      |
| 81 | system.bus.event_1      |

----
## Channels
The `channels` table holds information about all SystemC channels
(`sc_prim_channel`) present in the simulation. For each channel, it stores the
following fields:
* `id` (`BIGINT`) globally unique identifier
* `name` (`STRING`) full hierarchical name for each channel
* `kind` (`STRING`) channel description

Example table:
| id | name            | kind           |
|:--:|:---------------:|:--------------:|
| 23 | `system.wire_0` | `sc_signal`    |
| 90 | `system.irq_4`  | `sc_signal`    |

----
## Elaboration
The `elab` table holds information about modules starting and finishing their
setup phases during simulation elaboration.
* `rt` (`BIGINT`) real time stamp in nanoseconds when the event occurred
* `module` (`BIGINT`) globally unique identifier for the corresponding module
* `phase` (`INTEGER`) corresponding elaboration phase:
    - 0: `CONSTRUCTION`
    - 1: `BEFORE_END_OF_ELABORATION`
    - 2: `END_OF_ELABORATION`
    - 3: `START_OF_SIMULATION`
* `status` (`INTEGER`) status of the corresponding elaboration phase:
    - 0: phase started
    - 1: phase finished

Example table:
| rt     | module | phase | status |
|:------:|:------:|:-----:|:------:|
| 218964 |  1477  | 1     | 0      |
| 219455 |  1477  | 1     | 1      |
| 226977 |   812  | 2     | 0      |
| 231004 |   812  | 2     | 1      |

----
## Scheduling
The `sched` table holds information about SystemC processes resuming and
yielding their execution during simulation.
* `rt` (`BIGINT`) real time stamp in nanoseconds when the event occurred
* `proc` (`BIGINT`) globally unique identifier for the corresponding process
* `status` (`INTEGER`) status of the corresponding process:
    - 0: process has resumed execution
    - 1: process has yielded execution
* `st` (`BIGINT`) simulation time stamp in picoseconds when the event occurred

Example table:
| rt     | proc  | status | st       |
|:------:|:-----:|:------:|:--------:|
| 554964 |  237  | 0      | 10001000 |
| 559677 |  237  | 1      | 10001000 |
| 566977 |  602  | 0      | 20846000 |
| 571004 |  602  | 1      | 20846000 |

----
## Notification
The `notify` table holds information about SystemC event notifications and
cancellations.
* `rt` (`BIGINT`) real time stamp in nanoseconds when the event occurred
* `event` (`BIGINT`) globally unique identifier for the corresponding event
* `kind` (`INTEGER`) type of notification:
    - 0: immediate notification
    - 1: delta notification
    - 2: timed notification
    - 3: event cancellation
* `st` (`BIGINT`) simulation time stamp in picoseconds when the event occurred
* `delay` (`BIGINT`) notification delay for timed notifications, zero otherwise

Example table:
| rt     | event  | kind | st       | delay   |
|:------:|:------:|:----:|:--------:|:-------:|
| 469155 |  3401  | 0    | 10001000 | 0       |
| 501220 |  4642  | 1    | 10001000 | 0       |
| 543002 |   321  | 2    | 20445000 | 1000000 |
| 890211 |   321  | 3    | 21446000 | 0       |

----
## Channel Updates
The `updates` table holds information about SystemC channel updates while the
simulation is running.
* `rt` (`BIGINT`) real time stamp in nanoseconds when the event occurred
* `channel` (`BIGINT`) globally unique identifier for the corresponding channel
* `status` (`INTEGER`) status of the update:
    - 0: channel has started its update
    - 1: channel has completed its update
* `st` (`BIGINT`) simulation time stamp in picoseconds when the event occurred

Example table:
| rt     | channel | status | st       |
|:------:|:-------:|:------:|:--------:|
| 123430 |  19     | 0      | 0        |
| 123979 |  19     | 1      | 0        |
| 590927 |  812    | 0      | 22656010 |
| 598001 |  812    | 1      | 22656010 |
