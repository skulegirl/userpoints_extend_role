Copyright 2014 Anna Maste

Descriptrion
------------
Add a configurable amount of time to a user's role expiry date, when
the number of points reaches a certain threshold. The threshold may
be a multiple of a specfied number of points, e.g. every 30 points.
The points can also be restricted to a certain category, e.g. only 
for points with the taxonomy "User Referral" term attached.

For example:

The "full member" role's expiry date is to be extedned by 1 month for
every 30 points from the "user referral" category. If a user currently has
20 points and is awarded another 10 points, the role extension will be 
awarded, assuming the user currently has an expiry date set for the "full
member" role. (If not, it will fail silently). If the user currently has 
only 10 points, no extension will be awarded.

The total number of points can be configured to be decremented by the 
award threshold if desired, or the points can be left alone for 
record-keeping purposes.

The number of extensions awarded is kept track of and can be viewed for any
given user and role. 
