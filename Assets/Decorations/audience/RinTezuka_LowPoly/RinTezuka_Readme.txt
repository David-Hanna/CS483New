1. how to switch the model to respond to the biped rig, instead of the custom one?

Select the model through unfreezing it, or via the layer panel and you can see it has TWO skin modifiers. Turn the custom one off, and the biped one on.



2.how to transfer your animation from the custom rig to the biped?

The workflow for this is, after animating the custom rig, backup your project becouse this is destructive changes. 

Then select all objects by the layer named "bones -> bip" 

(you can keep it hidden, just click right-click and select all of it trough the layer window.)

then go to motion panel, trajectories, and bake the motion (via collapse transform ) with maybe twice as much samples for every frame then you really needed. (that sometimes helps the glitches)

go to animation - save animation and save it as a file.

Then swich Rin's model to be skinned to the biped instead of the custom rig. (explained in question one)

Select the biped rig, all thats in the layer named "BipedRig" and go to animation - load animation.

Now you can load your file to the biped.

You will need to either create an animation mapping file, or try to use one in the folder "aditional" -->"Rin_custom2bip_Mapping", it tends to bug out sometimes, just try again, or create your own mapping file.

When you create your own, just select all the bones, on the left, and the right and click either closest name, or exact name, i really am not sure how to make it work flawlessly, but i get it to work eventually every time by trial and error.



3.Whats up with the weird Spine in the custom rig?

Its ment to be this setup where you can animate the hips independently from the shoulders, for things like bellydancing, for example. Or simple walkcycle, with swaying hips. Biped would never be able to do that. You can also animate modifiers on the spine object itself, meaning you can have a perfect wave animation in the spine. Look at the walkcycle animation and you'll see how its used there.



Tips:



*Be sure to have the textures antializing set on "summed area", it stretches a little better then, when the eyes are closed.
*the model is ment to be rendered in scanline and the texture has the lighting baked in it. So it has no shadowing on, not even specular, highlights, no shading at all. But you can lower the self illumination of the material to 50 and have a bit of shading, however, the model is not really done with that in mind, so she might start looking weird.


known bugs:

1. Her left Eye sometimes seems to flip 180 degrees when you remove all keyframes via biped. It also messes the rotation of the shoulders. Going into anatomy mode removes the shoulder glitch, but to fix the eye load a pose from the folder containing Biped saved poses for her.

2. The tie sometimes does weird stuff and also likes to intersect with her chest. Realize you can not only rotate, but also move the Xtra bone for the tie and correct problems by hand that way.

3. As I said, when transfering motion from the custom rig to biped, it can have all sorts of bugs. It's usually due to the mapping file. I usually create a new one, choosing all of the bones to the left, then all of them to the right and click match by closest name, or exact name, up to the left. Then I save that mapping file, and try loading again, even multiple times. It really sometimes starts to work all of the sudden. Or I create another mapping. Basically, if someone knows how to do this properly, let me know. :-)

