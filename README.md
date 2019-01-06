C++ Road signs detector by color and textural features
=========================

## Usage
This game is based on the engine of the "Playrix" company.

## Description
The application is a shooter in which the main objects are targets destroyed by bullets from a user-controlled weapon. 
The player is given 50 seconds to destroy 20 targets with a pistol, in which there is a magazine for 20 rounds.
All these and other attributes are set in the input.txt configuration file.
In the case when the player ends the cartridge, he needs to press the "R" (recharge) key. This will tell him a pop-up texture with an empty store.
If a player has a time to destroy all targets, or he runs out of time, then the corresponding texture, symbolizing victory or defeat, is shown. To restart the game, you must press the button «B» (begin).

The following modules are implemented in this application:
1. ShooterDelegate. Connecting widgets.
2. ShooterWidget. The main widget of the application.
3. ObjectsPool class. Required to manage targets: their destruction from memory, adding new ones, etc. It is through this class that the main widget interacts with the targets.
4. IObjectForShot class. Target description class. The base class, which implements the mechanics of interaction of targets, simple movement of each of them, and also describes the main attributes. Classes-heirs set these attributes, plus there is an opportunity for redefining key mechanics or adding your own. There are implemented two classes in this app: Bomb and SuperBomb.
5. MachineGun class. Required to control bullets: adding them to the store, moving bullets into the container, as well as destroying existing ones. Through this class, the interaction of the widget with bullets.
6. Bullet class. Bullet description class. The base class, which implements the mechanics of the movement of bullets, their animation and interaction with targets. A PistolBullet inheritance class has been implemented, which describes the attributes of a pistol bullet.
7. Aim class. Class description of the mechanics of sight at the gun.


This architecture is designed to encapsulate the mechanics of the actions of objects in highly specialized classes, but also to provide a convenient way to add new objects (both bullets and targets).
To add a new target, it is enough to simply inherit from the IObjectForShot base class and set individual attributes. It is also necessary to correct the order of adding targets in ObjectsPool.
Similarly, with the addition of new types of bullets, it is enough to implement a descendant class from the Bullet class and specify the attributes.
A possible optimization of the architecture would be to create an abstract class whose implementation would be the ObjectsPool and MachineGun classes. You can also come from the IObjectForShot and Bullet classes.
