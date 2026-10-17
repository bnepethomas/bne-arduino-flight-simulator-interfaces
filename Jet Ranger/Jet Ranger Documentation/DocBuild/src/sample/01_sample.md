# System overview

The Jet Ranger panel is driven by **one PC** over Ethernet. This paragraph checks *italic*, **bold**, and `inline code` such as `D,PITCH:4.5,BANK:-12.0`.

![The whole system on one page](diagrams/system_overview.svg)

## Boards and addresses

| Board | IP | Role |
|---|---|---|
| Radio controller | 172.16.1.101 | COM/NAV displays, knobs, buttons |
| Servo controller | 172.16.1.102 | attitude servos, lamps |
| Stepper controller | 172.16.1.105 | seven gauges |

### Rules

- Gauges are powered from the main bus at 20 V or more.
- The Radio uses the avionics bus at 24 V or more:
  - if the avionics bus reads 0 the main bus is used
  - otherwise it uses the avionics bus
- Everything else follows.

1. First step.
2. Second step with `code`.
3. Third step.

> **Note:** callouts are used for cautions and tips.

```
D,PITCH:4.5,BANK:-12.0,EOUT:0
C,RESET:1
```
