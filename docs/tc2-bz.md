
# Data Providers

The data providers are tasks which primarily focus on talking directly with the train controller to issue commands or poll on sensor data. These tasks form into three major groups.

## TrainProvider

The train provider entity is a set of tasks which performs the operations of sending atomic train commands to the train controller. The train controller listens on any incoming requests made by other tasks and sends the request to the train controller. In addition, the TrainProvider raises a raw event to the pipeline signalling that an event to move a train has been issued. Subscribers on the raw event are notified when the event takes place. Currently, only the WaitingRoom task subscribes to the TrainProvider.

## SwitchProvider

Similar to the TrainProvider, the SwitchProvider is a set of tasks performing sends and raising of events for when switch requests occur, The SwitchProvider listens on any incoming requests and executes them. It raises an event and notifies all subscribers that a raw event for a change on a specific switch has occured. Currently, only the WaitingRoom task subscribes to the SwitchProvider

## SensorProvider

The SensorProvider is a set of tasks which manage the polling of sensor data. All round trips of sensor data are raised as a raw event for the subscribers to listen for. On a further note the WaitingRoom which subscribes to the SensorProvider sits behind a SensorDelta, which filters the raw events for only the iterations which differ from its previous.

## VirtualEventProvider

Along side the raw data provider, TheVrtualEvent provider raises virtual events to subscribers. Virtual event registrations are periodically delivered to subscribers. Note that VirtualEvents are not affiliated with TrainController itself, but can provide accurate measurements and predictions for when Raw Events may actually occur.

# Waiting Room

The waiting room is a precursor to the interpreter. Many Virtual Events have direct correlations with a Raw Event. These virtual events will wait in the waiting room for the corresponding raw events to occur. At the worst case, raw events which do not occur (ie broken sensor) can be pseudo reflected by the virtual event and will be up to the interpreter to decide an acceptable state.

## Event Types

The waiting room will return different event types depending on the situation of the anticipated event. Virtual Events can pre-emptively register to the waiting room that the virtual event is coming up known as a VRE. When a raw event corresponding to the virtual event, it raises an RE. When the virtual event comes in, a VE is raised. Combining the three, we obtain the combinations (VRE VE), (VRE RE), (VRE VE RE). 

```VRE VE``` - Virtual Event came in, timed-out without a corresponding Raw Event
```VRE RE``` - Virtual Event Registered, Raw Event came before estimated Virtual Event
```VRE VE RE``` - Virtual Event came, Raw Event came shortly after (within an alloted threshold)

# Acceleration, Velocity and Distance

In order to modularize at a higher precision based on train velocity.



