# Scout

_Scout_ is an open-source project consisting of hardware and firmware that enables software developers, drone manufacturers and commercial drone pilots to quickly develop drone solutions that collect (using an extensible chip architecture) information about the location of the device (or, the vehicle to which the device is attached), and use cellular communications technologies to both send that position and other sensor data to and receive navigation data back from unmanned traffic management (UTM) system.

Scout's *not* presently aimed at drone pilots or consumers who want an off-the-shelf device to attach to their drone. While a few such devices do exist, we created _Scout_ to address several limitations of these devices: that they're proprietary, work only with a specific service provider (which supports vendor lock-in and reduces market choice), and are generally only 'one-way' (i.e. broadcasting the drone's location over the cellular network to a remote receiver).

_Scout_ is designed to work _in tandem_ with the flight controller or ground control station already responsible for the flight of the drone itself, by augmenting the capability of the control solution with additional information obtained from an unmanned traffic management system, like GuardianUTM by Altitude Angel.

## Why we built _Scout_

With _Scout_, it's possible to connect a drone to a UTM Provider with secure, two-way communications and to _receive_ important safety messages back from the UTM that the drone's control station can respond to, such as:

* Changes to the airspace while you fly (eg. the creation of a NOTAM in-flight)
* Severe weather forecast changes
* Other nearby drones operating in the vicinity (traffic alerts)
* The presence of nearby manned aviation likely to conflict with your current flight path (conflict resolution)

While proprietary devices on the market exist for sending telemetry about a drone's location to a remote provider, that's only a small piece of the overall landscape of services required to support large-scale automated drone operations. Equally, these devices typically only work with a single provider (known as vendor lock-in, which does not facilitate an ecosystem to flourish), and are closed-source. What is necessary to support routine, automated beyond visual line-of-sight drone flights at scale is a richer connection into new and emerging digital traffic management services using an open architecture that can be enhanced and properly scrutinized.

### Scout uses the hardware ecosystem trusted by tens of millions of makers globally

We therefore started _Scout_ on the following principles:

* Use an existing open-source hardware ecosystem
* Use readily-available, 'commercial off-the-shelf' hardware components to minimise build cost, and risk
* Simplify drone connectivity to a UTM and make it open and available to everyone
* Ensure connectivity is two-way, drone to UTM _and_ UTM to drone

_Scout_ is built using the well-developed and well-known [Arduino](https://www.arduino.cc/) ecosystem and commercially available 2G/3G/4G and GPS modem modules. Current support is for the 8-bit AVR microcontroller ([ATMEGA328P](https://www.microchip.com/wwwproducts/en/ATmega328)) in the Arduino Uno and Nano, and the SIMCOM [SIM7100E](https://simcom.ee/modules/lte/sim7100e/) LTE/GNSS modem module, and with the community's help, we'll continue to add support for a wider range of chips and devices in the future.

## Getting Started

:warning: Before you build and fly with Scout, you should keep in mind that in most jurisdictions it is a requirement to keep your drone in your visual line-of-sight at all times. It is important that you use Scout in accordance with local laws, and that you test Scout is suitable before your first flight.

We recognize that most people will want to get up and running quickly. Whilst we love that, it's important to remember that _Scout_ is currently primarily targeted at non-retail users: those with programming and hardware backgrounds, such as manufacturers of ground control systems (GCS), autopilot/flight controller hardware and drone manufacturers themselves. Accordingly, in the earlier versions of the hardware and software, there are a few small hoops you'll need to jump through in order to get up-and-running, and, as in all early-release projects, we're continually adding improvements and tweaks along the way and both welcome and encourage community contributions, too.

### Step one: build your hardware

For now, the shortest sequence of steps to go from this repository to a device that you can fly around with is as follows:

* Build the [hardware](hardware/README.md) and upload the [firmware](firmware/README.md) to the microcontroller
  * [Breadboard Build Guide](hardware/breadboard/BreadboardBuildGuide.md)
  * [Standalone Hardware Build Guide](hardware/pcb/PCBBuildGuide.md)
  * [Case Build Guide](hardware/case/CaseBuildGuide.md)
* You'll also need a SIM card with a data contract from your preferred mobile network operator (noting that not all telecoms operators permit the use of their network at altitude, so do check first).

> Note that you don't _need_ Scout hardware to connect to the UTM Providers listed in this repository. In fact, if you already have a means to provide an internet connection back from your drone to the UTM Provider service, at relatively low latency, then you can skip this part and move straight to software integration.

### Step two: Connect to a supported UTM Provider

_Scout_ is designed to collect location data (which we call a 'position report') and forward it to a UTM system provider over cellular connectivity, working as a 'co-pilot' to help your drone(s) avoid hazardous situations while in-flight. To do this effectively, it's best if you connect it to a UTM Provider that has the necessary APIs and infrastructure in place to receive the telemetry, and provides the capability to send data back to your _Scout_ device (or via another control channel) so that you can respond to the presence of other nearby drones or manned aviation etc.

> Did you know? You can actually build-out your own back-end UTM services if you wish, but you'll need to keep in mind that building scalable infrastructure that's reliable enough to deal with your drone while it is in transit is an important consideration not to be overlooked, as are the services which you'll need to build on top of that infrastructure.

#### Supported UTM Providers

The current firmware supports connectivity into the following UTM Providers:

| UTM Provider | 1-way | 2-way |
| --- | --- | --- |
| [Altitude Angel](https://developers.altitudeangel.com) | Yes | Yes |

NB: You can contribute your own code to provide connection to other UTM Providers, or you can modify the firmware to work directly with your own, non-UTM based system. The choice is yours!

## Roadmap
The initial release of Scout is about providing a low-cost, workable combination of hardware and software to bring UTM to life in real-world scenarios. _Scout_ is based on Altitude Angel's core values of openness and choice in the market, which is why it is offered as open source. Accordingly, you can hook it up to work in almost any scenario and with any cloud platform provider.

### What's next?

The project is currently in release version 0.1, however, we've already set out our next few steps and are encouraging the community to support the project's development further. Below are what we're planning to add support for next:

* Adding I2C bus integration with Altitude Angel's Tactical Conflict Resolution Service (CRS) API
* Support for additional GPS/2G/3G/4G modules
* Hardware designs for a self-contained, battery powered, 3D printable unit
* Additional sensors support including barometer, temperature, humidity, accelerometer, and magnetometer
* Integration with open source drone avionics projects

You can help to shape and influence the direction and roadmap for _Scout_, simply by using it, sharing your feedback and ideas with the rest of the contributors and the community.

# License

The Scout firmware is licensed under the [Apache 2.0 licence](https://choosealicense.com/licenses/apache-2.0/#), and the hardware is licensed under [Creative Commons BY-SA](https://creativecommons.org/licenses/by-sa/4.0/).

Made with 💗 by Altitude Angel
