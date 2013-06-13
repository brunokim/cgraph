===================================================================
==                                                               ==
== Twitter message collection for the Spanish 15M movement       ==
==                                                               ==
== Contact:                                                      ==
==                                                               ==
==   Yamir Moreno             (yamir.moreno@gmail.com)           ==
==   Javier Borge-Holthoefer  (borge.holthoefer@gmail.com)       ==
==                                                               ==
===================================================================


-----------
Description
-----------
15m.zip contains this "readme.txt" plus two files with information about Twitter messages that were collected in the period April-May 2011. These messages are related to the political events occurred at that time in Spain, and only messages in Spanish are considered. A detailed description of the data can be found in [1], [2] and [3]. 

Important note: The data were harvested by Cierzo Development Ltd., a local start-up company, and we are held by a privacy agreement. Some information can not be disclosed. Thus, whatever features NOT present in these public files are those which (a) we never had access to; or (b) we are not allowed to make public.


--------
Files
--------

****** 15m.csv: 
This file reflects the activity around the 15M movement. The file is organized in three fields, separated by ";"

idt;segs;hashtags;mentions

- "idt" refers to the user id, i.e. the Twitter account from which the message was delivered. This "idt" is conveniently anonymized (to an integer number) to preserve users' actual identity.
- "segs" refers to the time at which the message was delivered, in seconds. The time t=0 (first collected message) corresponds to April 25th, 2011 at 00:18:27.
- "hashtags" lists all the hashtags contained in the message. Note that such list is variable, and some care is needed when processing the file.
- "mentions" lists any other user that was mentioned within the message in the form of '@user' (the numerical id is provided). Again, the field has a variable length (depending on the amount of mentions made in that particular message) and NULL indicates that no mentions were made.

Please cite [1] and/or [2] if this file is used.


****** 15m_follower.net: 
This file is in Pajek format, encoding the directed, unweighted network structure of "follower-following" relationships between pairs of users. Users are assigned a numerical id, ranging from 1 to 87569, which corresponds with the id in the previous file (15m.csv).

Note 1: As described in [2] and [3], link direction corresponds to the direction of the information flow: a node has out-going links towards those who follow him; and in-coming links from those whom he follows.
Note 2: The network does not contain the whole set of following-follower relationships (which would yield a structure with over 3 million nodes). We have filtered these relationships only to those who participated in the 15M events (thus, those who sent at least a message in that topic; or those who did not send any message, but were mentioned with a '@' symbol by someone who did.

Pajek format:

*Vertices 87569
1 *64
2 *65
3 *66
4 *67
5 *68
…
*Arcs
1 4464 1
1 21243 1
1 27205 1
1 32226 1
1 40335 1
2 3089 1

The first part of the file (below "*Vertices") describes the nodes in the network. You may ignore the second field (which is supposed to contain the node's tag, but is useless here).
The second part of the file (below "*Arcs") contains the links of the network in the format "from to weight". As the weight is always 1 (unweighted network), you may also ignore this field.

Please cite [2] and/or [3] if this file is used.


----------
References
----------

[1] Structural and Dynamical Patterns on Online Social Networks: the Spanish May 15th Movement as a Case Study. J. Borge-Holthoefer et al. PLoS One 6(8): e23883 (2011) doi:10.1371/journal.pone.0023883

[2] The Dynamics of Protest Recruitment through an Online Network. S. González-Bailón, J. Borge-Holthoefer, A. Rivero and Y. Moreno. Scientific Reports, 1, 197 (2011) doi: 10.1038/srep00197

[3] Locating Privileged Spreaders on an Online Social Network. J. Borge-Holthoefer, A. Rivero and Y. Moreno. Physical Review E, 85, 066123 (2012)

