# BLE Service Discovery with Zephyr

Demo code to accompany my talk on the Zephyr Meetup in Hamburg.

## Components

This is a multi-app repository, which is a bit unusual for Zephyr. The `west` manifest is in
`manifest/west.yml`, but each app has its own source directory.

- `peripheral`:
  An example peripheral providing two simple services. This is the target of the actual demo apps.
