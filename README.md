## Sensor Fusion Nanodegree


### Camera Mid Term Project Report

### MP.1 Data Buffer Optimization

The Data Buffer was implemented as follows:

``` c++
dataBuffer.push_back(frame);
if (dataBuffer.size() > dataBufferSize)
  dataBuffer.erase(dataBuffer.begin());
```

### MP.2 Keypoint Detection

Keypoint Detection was implemented in the src/matching2D_Student.cpp file in:

- detKeypointsHarris
- detKeypointsModern

The appropriate detector can be selected by modifying the value:

``` c++
    string detectorType = "HARRIS";               // SHITOMASI, HARRIS, FAST, BRISK, ORB, AKAZE, SIFT
```

In addition a boolean variable was defined in order to iterate through all detector and decriptor combinations. Setting to true iterates through all. Setting to false will use the value set in the detectorType string.
```
    bool bProcessAll = true;
```

### MP.3 Keypoint Removal

The keypoints outside the defined region of interest (aproximating to an area around the lead vehicle) were removed using the code:

``` c++
bool bFocusOnVehicle = true;
cv::Rect vehicleRect(535, 180, 180, 150); // topleft x, topleft y, width, height

if (bFocusOnVehicle)
{
    // to help debugging
    int eraseCnt = 0;
    int keepCnt = 0;
    for (auto it = keypoints.begin(); it < keypoints.end();)
    {
        int thispointx = it->pt.x;
        int thispointy = it->pt.y;
        if ((thispointx < vehicleRect.x) || (thispointx > (vehicleRect.x + vehicleRect.width)) || (thispointy < vehicleRect.y) || (thispointy > (vehicleRect.y + vehicleRect.height)))
        {
            // erase returns pointer to next element
            it = keypoints.erase(it);
            eraseCnt++;
        }
        else
        {
            keepCnt++;
            it++;
        }
    }
    cout << "vehicleKeypoints: " << keypoints.size() << ", ";// << endl;
}
```

In hindsight, this could have been implemented quicker using contains() as mentioned in one of the video lessons.


### MP.4 Keypoint Descriptors

The descripors were implemented in:

- descKeypoints

The descriptor Type to be used can be selected by modifying:

``` c++
    string descriptorType = "SIFT";     
```

As before, the bProcessAll boolean can be used to enable iteration through all valid combinations. Setting to false will use the value set in the descriptorType string.
    ```
        bool bProcessAll = true;
    ```

### MP.5 Descriptor Matching

Flann and k-nearest neighbour (KNN) are both implem in matchDescriptors and selectable via:

``` c++
string matcherType = "MAT_FLANN";             // MAT_BF, MAT_FLANN
string selectorType = "SEL_KNN";              // SEL_NN, SEL_KNN
```

### MP.6 Descriptor Distance Ratio

Descriptor Distance Ratio filtering is implemented in matchDescriptors

``` c++
double minDescDistRatio = 0.8;
for (auto it = knn_matches.begin(); it != knn_matches.end(); ++it)
{
    if ((*it)[0].distance < minDescDistRatio * (*it)[1].distance)
    {
        matches.push_back((*it)[0]);
    }
}
```


### MP.7 Performance Evaluation 1

<em> Count the number of keypoints on the preceding vehicle for all 10 images and take note of the distribution of their neighborhood size. Do this for all the detectors you have implemented. </em>

The performace of each of the keypoint detectors was evaluated. Only the key points in the Region of interest specified were counted, but time measurements were taken for the dtection of keypoints across the complete image. The total number of keypoints detected across all images was summed, along with the summed period taken for the detcion step on each image.

The table below shows the total number of keypoints generated per detector (I.e the sum of the keypoints generated in each of the 10 images)

The keypoints were plotted onto the images and viewed for assessment.
The application was updated to allow automatic processing of valid combinations, outputing of the results to the terminal in a format that could be saved as a .csv file for further processing.
The saved images can be viewed in the
[output folder](output/)

For each input image a smaller cropped detector is included. Note that the cropped version is larger that the specified ROI, to ensure that the keypoints, including their neighbourhood area can be rendered and viewed.




<table class="tg">
<thead>
  <tr>
    <th class="tg-0pky">Detector</th>
    <th class="tg-0pky">Keypoints in ROI (lead vehicle)</th>
    <th class="tg-0pky">Keypoint volume rank</th>
    <th style="width:300px">Sample image</th>
    <th class="tg-0pky">Comments</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td class="tg-0pky">BRISK</td>
    <td class="tg-0pky">2722</td>
    <td class="tg-0pky">1</td>
    <td class="tg-0pky"><img src="/output/BRISK/brisk.gif" alt="brisk"/> </td>
    <td class="tg-0pky">Detects the largest number of keypoints. Thepoint have the largest meaningful neighbourhood. The majority of the keypoints are clustered around the outside of the lead vehicle, but also with some keypoints around the number plate and light clusters,  .Keypoints were also detected on the vehicles in the distance, both in the same lane as the ego vehicle and adjacent lanes</td>
  </tr>
  <tr>
    <td class="tg-0pky">AKAZE</td>
    <td class="tg-0pky">1666</td>
    <td class="tg-0pky">2</td>
    <td class="tg-0pky">
<img src="/output/AKAZE/akaze.gif" alt="akaze"/></td>
    <td class="tg-0pky">Akaze detectsfewer keypoints than BRISK, with much smaller neigbourhhoods. There is a significant concentration around the outside of the lead vehicle, with a fairly uniform distribution in these areas. There are no keypoints detected in areas such as the number plate.Again the vehicles in adjacent vehicles have keypoints detected.</td>
  </tr>
  <tr>
    <td class="tg-0pky">FAST</td>
    <td class="tg-0pky">1501</td>
    <td class="tg-0pky">3</td>
    <td class="tg-0pky"><img src="/output/FAST/fast.gif" alt="FAST"/> </td>
    <td class="tg-0pky">FAST generates fewer keypoints, but is the fastest. There is again a decent concentration of keypoints around the outer of the lead vehicle. It can be clearly seen from the image that keypoints are detected on other vehicles and lane markings.</td>
  </tr>
  <tr>
    <td class="tg-0pky">SIFT</td>
    <td class="tg-0pky">1381</td>
    <td class="tg-0pky">4</td>
    <td class="tg-0pky"><img src="/output/SIFT/sift.gif" alt="sift"/></td>
    <td class="tg-0pky">SIFT detects keypoints on the rear screen of the proceeding vehicle. The size of the neighbourhoods vary from very small to medium size. </td>
  </tr>
  <tr>
    <td class="tg-0pky">SHITOMASI</td>
    <td class="tg-0pky">1189</td>
    <td class="tg-0pky">5</td>
    <td class="tg-0pky"><img src="/output/SHITOMASI/shitomasi.gif" alt="shitomasi"/> </td>
    <td class="tg-0pky">Shi-tomasi generates a cluster of keypoints around the number plate of the lead car, around the headrests visible through the rear screen and the edge of the vehicle.</td>
  </tr>
  <tr>
    <td class="tg-0pky">ORB</td>
    <td class="tg-0pky">1150</td>
    <td class="tg-0pky">6</td>
    <td class="tg-0pky"><img src="/output/ORB/orb.gif" alt="brORBisk"/> </td>
    <td class="tg-0pky">Keypoints are concentrated into a number of highly dense areas. The main concentrations appear to be around the lights. The keypoints are so concenrtated that the keypoint plots almost appear as concentric circles. The neighbourhood areas are the largest of the detectors. Orb returns a deifned number of keypoints each time which my default is set to 500. This was not modifed during this exerces. Orb was the 2nd fastest detector. ORB was one of the few detectors not to detect keypoints on the leading vehicles shadow.</td>
  </tr>
  <tr>
    <td class="tg-0pky">HARRIS</td>
    <td class="tg-0pky">248</td>
    <td class="tg-0pky">7</td>
    <td class="tg-0pky"><img src="/output/HARRIS/harris.gif" alt="harris"/> </td>
    <td class="tg-0pky">Harris generates the fewest number of keypoints. Because it detects fewer keypoints there are less keypoints on the other vehicles as well. On the lead vehicle, there are keypoint clusters around the brake lights (both side and centre high mounted), but relatively few on the edge of the vehicle</td>
  </tr>
</tbody>
</table>





### MP.8 Performance Evaluation 2

<em> Count the number of matched keypoints for all 10 images using all possible combinations of detectors and descriptors. In the matching step, the BF approach is used with the descriptor distance ratio set to 0.8. </em>

| combination     | Keypoint Matches | Total Keypoint <br> match attempted| keypoint match ratio |
|-----------------|------------------|----------------------------------------------------------|----------------------|
| SHITOMASI SIFT  | 937              | 1076                                                     | 0.871                |
| AKAZE SIFT      | 1277             | 1490                                                     | 0.857                |
| AKAZE AKAZE     | 1173             | 1490                                                     | 0.787                |
| FAST SIFT       | 1055             | 1357                                                     | 0.777                |
| SHITOMASI BRIEF | 822              | 1076                                                     | 0.764                |
| HARRIS SIFT     | 163              | 214                                                      | 0.762                |
| AKAZE BRISK     | 1113             | 1490                                                     | 0.747                |
| ORB SIFT        | 758              | 1025                                                     | 0.740                |
| AKAZE BRIEF     | 1081             | 1490                                                     | 0.726                |
| SHITOMASI ORB   | 775              | 1076                                                     | 0.720                |
| HARRIS ORB      | 145              | 214                                                      | 0.678                |
| BRISK SIFT      | 1638             | 2470                                                     | 0.663                |
| HARRIS BRIEF    | 141              | 214                                                      | 0.659                |
| FAST BRIEF      | 893              | 1357                                                     | 0.658                |
| AKAZE FREAK     | 976              | 1490                                                     | 0.655                |
| FAST ORB        | 886              | 1357                                                     | 0.653                |
| SIFT SIFT       | 807              | 1245                                                     | 0.648                |
| SHITOMASI BRISK | 697              | 1076                                                     | 0.648                |
| ORB BRISK       | 648              | 1025                                                     | 0.632                |
| AKAZE ORB       | 926              | 1490                                                     | 0.621                |
| FAST BRISK      | 790              | 1357                                                     | 0.582                |
| HARRIS FREAK    | 123              | 214                                                      | 0.575                |
| HARRIS BRISK    | 121              | 214                                                      | 0.565                |
| BRISK BRIEF     | 1355             | 2470                                                     | 0.549                |
| SHITOMASI FREAK | 582              | 1076                                                     | 0.541                |
| BRISK BRISK     | 1300             | 2470                                                     | 0.526                |
| ORB ORB         | 532              | 1025                                                     | 0.519                |
| FAST FREAK      | 680              | 1357                                                     | 0.501                |
| SIFT BRIEF      | 603              | 1245                                                     | 0.484                |
| BRISK FREAK     | 1099             | 2470                                                     | 0.445                |
| SIFT BRISK      | 543              | 1245                                                     | 0.436                |
| ORB BRIEF       | 446              | 1025                                                     | 0.435                |
| SIFT FREAK      | 512              | 1245                                                     | 0.411                |
| BRISK ORB       | 942              | 2470                                                     | 0.381                |
| ORB FREAK       | 346              | 1025                                                     | 0.338                |

### MP.9 Performance Evaluation 3

<em> Log the time it takes for keypoint detection and descriptor extraction. The results must be entered into a spreadsheet and based on this data, the TOP3 detector / descriptor combinations must be recommended as the best choice for our purpose of detecting keypoints on vehicles. </em>

A spreasheet including all data is at the link [report](report.ods)


| combination     | Total time (det+desc) |
|-----------------|-----------------------|
| FAST BRIEF      | 15.283221             |
| FAST ORB        | 17.569503             |
| FAST BRISK      | 26.662015             |
| ORB BRIEF       | 75.413866             |
| ORB ORB         | 99.79161              |
| HARRIS ORB      | 134.384381            |
| HARRIS BRIEF    | 135.70158             |
| HARRIS BRISK    | 141.093285            |
| ORB BRISK       | 144.04453             |
| SHITOMASI BRIEF | 144.519401            |
| SHITOMASI ORB   | 145.497365            |
| FAST SIFT       | 148.725341            |
| SHITOMASI BRISK | 162.96137             |
| SHITOMASI SIFT  | 230.4309              |
| HARRIS SIFT     | 251.0689              |
| BRISK BRIEF     | 415.70919             |
| FAST FREAK      | 420.729469            |
| BRISK BRISK     | 435.175               |
| BRISK ORB       | 441.56829             |
| ORB SIFT        | 448.55095             |
| ORB FREAK       | 504.79503             |
| SHITOMASI FREAK | 522.4053              |
| HARRIS FREAK    | 574.2756              |
| AKAZE ORB       | 580.38048             |
| AKAZE BRISK     | 618.2555              |
| AKAZE SIFT      | 704.5548              |
| BRISK SIFT      | 712.2969              |
| AKAZE BRIEF     | 747.686952            |
| BRISK FREAK     | 841.6666              |
| SIFT BRISK      | 975.52711             |
| AKAZE FREAK     | 984.4946              |
| AKAZE AKAZE     | 1027.9737             |
| SIFT BRIEF      | 1040.692737           |
| SIFT FREAK      | 1470.2517             |
| SIFT SIFT       | 1843.5979             |


#### Findings  & Recommendation

For automotive implementations, a good detector must be fast, but also be able to detect a large number of keypoints. Typically detectors are good at one or the other, which means that compromises must be made. In order to provide a truely objective proposal they should be evaluated against metrics for speed and performance.

##### Volume of Keypoints
Brisk returned the largest number of keypoints.
Following Brisk, the order of most keypoints to fewest was, AKAZE, FAST, SIFT, SHITOMASI, ORB, with all providing a reasonable number of keypoints.  
Harris detected very few keypoints, several magnitudes less than the others making it a poor option for the task.

##### Matching Performamce

Combinations that used SIFT, BRIEF and AKAZE had the highest percentage of keypoint matching. The highest percentage was 87% with the average 62%. Lowest performace percentages were down at 33% for Orb detector with FREAK descriptor.

##### Speed
In line with their name, the FAST detector provided the quickest results, when paired with BRIEF, ORB and BRISK descriptors.

##### Recommendation

As mentioned, the recommendation is a balance of priorities.

My recommendation would be:  
1 - FAST BRIEF  
2 - FAST ORB  
3 - FAST BRISK

The FAST BRISK combination has a slighly lower than average keypoint matching ratio, but it is still 3 times as fast as the 4th fastest performing combination (Orb Brief, which has a lower match ratio)

This is based on the speed gains being several orders of magnitudes better that the matching performance of the other combinations.
