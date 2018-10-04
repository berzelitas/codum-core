# Milestone Based Payment Standard

## Information about contract structure

### Schedule

The schedule struct has special fields:

* funding_status
* execution_status

The **funding_status** field will signal the status of the milestone funding, where:

* 0 = funding has not started yet
* 1 = milestone is funded at least 1% to project account
* anything in between - milestone funding to project account progress
* 100 = milestone development is fully funded to the project account
* 201 = milestone aborted/canceled, negotiating fund return
* 202 = milestone aborted/canceled, funds lost
* 203 = milestone aborted/canceled, some funds recovered
* 204 = milestone aborted/canceled, all funds recovered or it was not funded
* 250 = milestone approved for funding

The **execution_status** field will signal the status of milestone progress, where:

* 0 = execution has not started yet
* 1 = work on milestone objectives has started
* anything in between - milestone delivery progress
* 100 = full deliverable submission at 100%
* 101 = project is in review
* anything in between - review progress
* 200 = milestone approved, completed and closed

## Development information

### How to build with Docker

1. Build image: `docker build -t mbps-builder .`
2. Run building: ``docker run -v `pwd`/build:/build mbps-builder eosio-cpp -o /build/mbps.wasm mbps.cpp``
