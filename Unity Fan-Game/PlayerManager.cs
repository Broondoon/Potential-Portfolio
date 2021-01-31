using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// PlayerManager.cs, written by Brendan Wadey in C# for the Unity game engine.
//
// This is a simple player controller/manager script.
// It manages player movement, pushing objects and initiating dialogue.
//
// Movement follows a number system that requires explanation.
// Movement occurs every frame in the Update() function. Where and how the player moves is dictated by "move_state".
// "move_state = 0" means the player isn't moving and keyboard presses are accepted.
// "move_state = 1, 2, 3, or 4" means that the player should be currently moving in a direction.
// "move_state = 5" means that the player has ran into somehting, and needs to return to the original location (start_point).
// ...in the process of writing these comments, I noticed some refactoring opportunities and made this less complicated than it used to be...


public class PlayerManager : MonoBehaviour
{
    public GameObject eyebrows; //visual indicator of push-mode.
    public bool in_conversation = false; //boolean to signal when dialogue is happening and controls should change.

    private int move_state; //value which tracks movement.
    private Vector3 end_poz; //target location that the player should move towards when moving.
    private Vector3 start_poz; //location of player before they began moving.
    private Collider2D this_collider; //the collider of the player.

    // Start is called before the first frame update:
    void Start()
    {
        move_state = 0;
        end_poz = transform.position;
        start_poz = end_poz;
        this_collider = gameObject.GetComponent<Collider2D>();
        eyebrows.SetActive(false);
    }

    //React properly if we hit something when moving:
    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (move_state != 5)
        {
            this_collider.enabled = false; //turn off collider to gurantee that only one collision occurs

            //If it can be pushed and we're in push mode, push it!
            if (collision.gameObject.GetComponent<BumpScript>() != null && eyebrows.activeSelf == true)
            {
                collision.gameObject.GetComponent<BumpScript>().GetBumped(move_state); //tell the object that we've bumped it - it'd better move!
            }
            else
            {
                //If it can talk, talk to it.
                if (collision.gameObject.GetComponent<TalkScript>() != null)
                {
                    collision.gameObject.GetComponent<TalkScript>().StartTalking(); //tell the object we're talking to it.
                }
            }

            //Must return!
            move_state = 5; //set the movement to returning.
            end_poz = start_poz; //change the target position to be where we started from.
        }
    }

    // Update is used to move the player, one small bit every frame:
    void Update()
    {
        //Activate/deactivate push mode with the space bar.
        if (Input.GetKey(KeyCode.Space))
        {
            eyebrows.SetActive(true);
        }
        else
        {
            eyebrows.SetActive(false);
        }

        //Don't interrupt a movement in progress, and no moving when in conversation!
        if (move_state == 0 && in_conversation != true)
        {
            //Detect desire to move
            if (Input.GetKeyDown("up"))
            {
                move_state = 1; //set the direction of movement.
                start_poz = transform.position; //set the starting point to be where the player is before moving.
                end_poz = transform.position + Vector3.up; //set the taret point to be 1 unit away (here, 1 unit up).
            }
            else if (Input.GetKeyDown("left"))
            {
                move_state = 2; //right now the distinction between 1, 2, 3 and 4 is nothing, but I might need this later in developement.
                start_poz = transform.position;
                end_poz = transform.position + Vector3.left;
            }
            else if (Input.GetKeyDown("down"))
            {
                move_state = 3;
                start_poz = transform.position;
                end_poz = transform.position + Vector3.down;
            }
            else if (Input.GetKeyDown("right"))
            {
                move_state = 4;
                start_poz = transform.position;
                end_poz = transform.position + Vector3.right;
            }
        }

        //Move if we need to move.
        if (move_state != 0)
        {
            transform.position = Vector3.Lerp(transform.position, end_poz, 0.1f); //this built-in Unity function moves the player smoothly

            //Stop the lerp, if we're within a very small distance to our destination
            if (Vector3.Distance(transform.position, end_poz) <= 0.01)
            {
                transform.position = end_poz; //shift player to exact location - no one should notice!
                move_state = 0; //allow movement again.
                this_collider.enabled = true; //re-enable the collider if it was disabled from a collision.
            }
        }
    }
}