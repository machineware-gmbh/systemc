# InSCight Table Description

The following tables are used to store the tracing data. If SQL is chosen as
the database backend, all tables are stored in a single sqlite3 database called
`sim.<procid>.db`. If CSV is chosen as the database backend, each tables is
stored as its own CSV file called `<table>.<procid>.csv`.

----
## Meta
The `meta` table holds meta information about the SystemC simulation, such as
process-id, time-stamp, user-name, etc. Only one data entry will be present.
* `pid` (`BIGINT`) os-specific process-ID of the simulation
* `path` (`STRING`) os-specific path to the simulation binary
* `user` (`STRING`) username of the user who started the simulation
* `version` (`STRING`) version string of the employed SystemC implementation
* `timestamp` (`DATIME`) unix-timestamp when the simulation was started

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

----
## Port Bindings
The `bindings` table holds information about which SystemC ports have been
bound to which other port during elaboration.
* `from_port` (`BIGINT`) ID of the binding port
* `to_port` (`BIGINT`) ID of the bound port
* `kind` (`BIGINT`) Kind of binding (0: `NORMAL`, 1: `HIERARCHICAL`)
* `proto` (`BIGINT`) Protocol ID of the ports

| proto | description            |
| :---: | :--------------------: |
| 0     | unknown protocol       |
| 1     | `sc_signal<T>`         |
| 2     | TLM sockets            |
| 3     | VCML GPIO protocol     |
| 4     | VCML CLK protocol      |
| 5     | VCML PCI protocol      |
| 6     | VCML I2C protocol      |
| 7     | VCML SPI protocol      |
| 8     | VCML SD protocol       |
| 9     | VCML serial protocol   |
| 10    | VCML virtio protocol   |
| 11    | VCML ethernet protocol |
| 12    | VCML CAN protocol      |
| 13    | VCML USB protocol      |

Example table:
| from_port | to_port | kind | proto |
| :-------: | :-----: | :--: | :---: |
| 1001      | 1003    | 1    | 2     |
| 1002      | 1004    | 0    | 3     |
| 4501      | 24222   | 0    | 9     |

----
## Transactions
The `transactions table holds information about transactions that have been
traced during the simulation:
* `st` (`BIGINT`): Simulation time stamp in picoseconds
* `dir` (`INTEGER`): Trace direction
    - 0: transaction traced on path to target
    - 1: transaction traced on return path from target
* `port` (`BIGINT`): ID of the port that recorded the trace
* `proto` (`INTEGER`): Protocol ID of the transaction (see table above)
* `json` (`TEXT`): JSON representation of the transaction payload

The interpretation of the `json` field is dependant on the protocol ID found
in `proto` and may also be empty for some protocols, e.g., `{}`.

----
## Log Messages
The `logmsg` table holds information about which log messages have been posted
during the simulation.
* `st` (`BIGINT`): Simulation time stamp in picoseconds
* `loglvl` (`INTEGER`): information log level:
    - 0: error messages
    - 1: warning messages
    - 2: informational messages
    - 3: debug messages
* `sender` (`TEXT`): name of entity that generated the log message
* `msg` (`TEXT`): the actual log messages

Example table:
|  st    | loglvl | sender    | msg           |
| :----: | :----: | :----:    | :-----------: |
| 123445 | 0      | my_module | A log message |

----
## CPU Idle Trace
The `cpuidle` table holds information when a CPU has entered and left its idle
state (wait-for-interrupt).
* `st` (`BIGINT`): Simulation time stamp in picoseconds
* `cpu` (`BIGINT`): ID of the corresponding processor
* `idle` (`INTEGER`): Idle state of the CPU (0:active, 1:idle)

Example table:
| st     | cpu     | idle  |
| :----: | :-----: | :---: |
| 12344  | 13033   | 1     |
| 25012  | 13033   | 0     |

----
## CPU Call Stack Trace
The `cpustack` table holds information about the CPU call stack that has been
sampled during the simulation:
* `st` (`BIGINT`): Simulation time stamp in picoseconds
* `cpu` (`BIGINT`): ID of the corresponding processor
* `level` (`INTEGER`): Level of the current stack frame
* `addr` (`BIGINT`): Stack frame program counter
* `sym` (`TEXT`): Name of the function currently executing (if available)

Example table:
| st       | cpu   | level | addr      | sym    |
| :------: | :---: | :---: | :-------: | :----: |
| 11223344 | 13033 | 0     | 0x8000120 | printf |
| 11223344 | 13033 | 1     | 0x8002030 | foo    |
| 11223344 | 13033 | 2     | 0x8000000 | main   |
| 20488000 | 13033 | 0     | 0x8000660 | memcpy |
| 20488000 | 13033 | 1     | 0x8000000 | main   |

This corresponds to two callstacks:
- main > foo > printf (recorded at 11223344ps for cpu module 13033)
- main > memcpy (recorded at 20488000ps for cpu module 13033)

