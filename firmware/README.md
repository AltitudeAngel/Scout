# Scout Firmware

[![Build Status](https://dev.azure.com/altangel/Altitude%20Angel/_apis/build/status/AA.Scout?branchName=master)](https://dev.azure.com/altangel/Altitude%20Angel/_build/latest?definitionId=260&branchName=master)

## Before you begin

This directory contains the _firmware_ for the _Scout_ open-source hardware device. Before you can use the firmware, you'll want to have already built the Scout device, either on a breadboard or a PCB. Head over to the [Scout Hardware](../hardware/README.md) section for schematics and a build guide before you begin.

:warning: Do keep in mind that this project is open-source, and is provided "as is". You should verify and test that the firmware is suitable for any specific purpose you have in mind before you use it.

## Getting Started

This guide assumes that you have built a working _Scout_ device and working data connection, and you're ready to upload firmware.

### Overview

_Scout's_ firmware is designed to securely authenticate itself with a remote service and then establish a two-way communication channel between the _Scout_ device and the remote service for the duration of a flight. The channel is two-way so that the remote service can receive telemetry from the device (which includes the device's GPS coordinates) and can then use the telemetry to send back additional data, such as a recommended manoeuvre to avoid a collision.

The possibilities for this type of service are limited only by the capabilities of the remote service sending and receiving data to and from the _Scout_ device, and the way the responses from the remote service are handled.

Below are a few examples of common usage scenarios:

* Tracking a fleet of drones
* Connecting to a UTM Provider to receive near real-time navigation assistance to help you avoid other aircraft, or even changes to airspace restrictions

When connecting to a UTM Provider, the UTM Provider will explain how their service works and what responses will be sent under what conditions, so that as the implementor of the _Scout_ device in your solution, _you_ can decide what action to take if the UTM Provider prompts you to take action to avoid some type of conflict.

In this getting started guide, we're going to initially cover a blend of both scenarios: first, authenticating your device with the Altitude Angel UTM Provider service, and second, exchanging telemetry with it in real-time. In firmware version 0.2, scheduled June, we'll then show you how to _respond_ to data the Altitude Angel service sends to help you avoid collisions, or flying in a restricted area (known as _realtime, dynamic geo-fencing_).

### Picking a UTM Provider

_Scout_ is designed to collect telemetry and forward it to a remote service using cellular connectivity, working as a 'co-pilot' to help your drone(s) avoid hazardous situations while in-flight. To do this effectively, it's best if you connect it to a UTM Provider that has the necessary APIs and infrastructure in place to receive the telemetry, and provides the capability to send data back to your _Scout_ device (or via another control channel) so that you can respond to the presence of other nearby drones or manned aviation etc.

#### Example implementation flow for basic 'network Remote ID' concept, using Scout and Altitude Angel (one-way)

![Basic implementation showing use of Scout and Altitude Angel to satisfy network Remote ID concept](doc/ScoutSimpleRemoteIDFlow.svg)

#### Example two-way communication flow using Altitude Angel for navigation assistance

![Basic implementation showing use of Scout and Altitude Angel to satisfy network Remote ID concept](doc/ScoutAdvancedScenario.svg)

>> For a detailed guide on each of the above processes, view the [Altitude Angel UTM Provider documentation](doc/AltitudeAngel.md) contained in this repository.

Alternatively, you can build-out your own capability, build your own connector to another UTM service, and connect the _Scout_ device to one of those.

#### Supported UTM Providers

The current firmware supports connectivity into the following UTM Providers:

| UTM Provider                                           | 1-way | 2-way | Getting started guide                                      |
| ------------------------------------------------------ | ----- | ----- | ---------------------------------------------------------- |
| [Altitude Angel](https://developers.altitudeangel.com) | Yes   | Yes   | [Connect Scout to Altitude Angel](doc/AltitudeAngel.md) |

NB: You can contribute your own code to provide connection to other UTM Providers, or you can modify the firmware to work directly with your own, non-UTM based system. The choice is yours!

## Roadmap

The initial release of Scout is about providing a low-cost, workable combination of hardware and software to bring UTM to life in real-world scenarios. _Scout_ is based on Altitude Angel's core values of openness and choice in the market, which is why it is offered as open source and without restriction. Accordingly, you can hook it up to work in almost any scenario and with any cloud platform provider.

### What's next?

The project is currently in release version 0.1, however, we've already set out our next few steps and are encouraging the community to support the project's development further. Below are what we're planning to add support for next:

* Adding I2C bus integration with Altitude Angel's Tactical Conflict Resolution Service (CRS) API
* Support for additional GPS/2G/3G/4G modules
* Hardware designs for a self-contained, battery powered, 3D printable unit
* Additional sensors support including barometer, temperature, humidity, accelerometer, and magnetometer
* Integration with open source drone avionics projects

You can help to shape and influence the direction and roadmap for _Scout_, simply by using it, sharing your feedback and ideas with the rest of the contributors and the community.

# License

The Scout firmware is licensed under the [Apache 2.0 licence](https://choosealicense.com/licenses/apache-2.0/#). 

Made with 💗 by Altitude Angel
