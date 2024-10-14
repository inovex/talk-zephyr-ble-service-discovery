# BLE Service Discovery with Zephyr

Demo code to accompany my talk on the Zephyr Meetup in Hamburg.

## Components

This is a multi-app workspace project, which is a bit unusual for Zephyr. The `west` manifest is in
`manifest/west.yml`, but each app has its own source directory.

- `central`:
  A demo application showing multiple ways to enumerate the services of a peripheral device.
- `peripheral`:
  An example peripheral providing two simple services. This is the target of the actual demo apps.

## Building

Follow the [Getting Started Guide](https://docs.zephyrproject.org/3.7.0/develop/getting_started/index.html),
but skip cloning the Zephyr source tree (step 5 of "Get Zephyr and install Python dependencies"),
as this is a workspace application (or rather, project). Instead, clone this repository and run
`west` on the manifest:

```
west init -l manifest
```

Then, download the zephyr source by running `west update`.
To conserve disk space, I like to use the `--filter` option to `git`:

```
west update -o=--filter=blob:none
```

If you installed `west` into a virtual environment, you need to install the Zephyr dependencies:

```
python3 -m pip install -r zephyr/scripts/requirements.txt
```

Now, you can build the applications (do not forget to install the SDK if you haven't done it yet!):

```
cd central && west build -b nrf52833dk/nrf52833
cd peripheral && west build -b nrf52833dk/nrf52833
```
