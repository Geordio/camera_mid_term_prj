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

For each image, the full size version and a smaller croped version is included. Note that the cropped version is larger that the specified ROI, to ensure that the keypoints, including their neighbourhood area can be rendered and viewed.




<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;}
.tg td{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;
  overflow:hidden;padding:10px 5px;word-break:normal;}
.tg th{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:14px;
  font-weight:normal;overflow:hidden;padding:10px 5px;word-break:normal;}
.tg .tg-0pky{border-color:inherit;text-align:left;vertical-align:top}
</style>
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
    <td class="tg-0pky"><img src="/output/BRISK/brisk.gif" alt="brisk"/>
   </td>
    <td class="tg-0pky">Detects the largest number of keypoints. Thepoint have the largest meaningful neighbourhood. The majority of the keypoints are clustered around the outside of the lead vehicle, but also with some keypoints around the number plate and light clusters,  .Keypoints were also detected on the vehicles in the distance, both in the same lane as the ego vehicle and adjacent lanes</td>
  </tr>
  <tr>
    <td class="tg-0pky">AKAZE</td>
    <td class="tg-0pky">1666</td>
    <td class="tg-0pky">2</td>
    <td class="tg-0pky">![AKAZE](/output/AKAZE/akaze.gif) </td>
    <td class="tg-0pky">Akaze detectsfewer keypoints than BRISK, with much smaller neigbourhhoods. There is a significant concentration around the outside of the lead vehicle, with a fairly uniform distribution in these areas. There are no keypoints detected in areas such as the number plate.Again the vehicles in adjacent vehicles have keypoints detected.</td>
  </tr>
  <tr>
    <td class="tg-0pky">FAST</td>
    <td class="tg-0pky">1501</td>
    <td class="tg-0pky">3</td>
    <td class="tg-0pky">![BRISK](/output/FAST/fast.gif) </td>
    <td class="tg-0pky">FAST generates fewer keypoints, but is the fastest. There is again a decent concentration of keypoints around the outer of the lead vehicle. It can be clearly seen from the image that keypoints are detected on other vehicles and lane markings.</td>
  </tr>
  <tr>
    <td class="tg-0pky">SIFT</td>
    <td class="tg-0pky">1381</td>
    <td class="tg-0pky">4</td>
    <td class="tg-0pky">![SIFT](/output/SIFT/sift.gif) </td>
    <td class="tg-0pky">SIFT detects keypoints on the rear screen of the proceeding vehicle. The size of the neighbourhoods vary from very small to medium size. </td>
  </tr>
  <tr>
    <td class="tg-0pky">SHITOMASI</td>
    <td class="tg-0pky">1189</td>
    <td class="tg-0pky">5</td>
    <td class="tg-0pky">![SHITOMASI](/output/SHITOMASI/shitomasi.gif) </td>
    <td class="tg-0pky">Shi-tomasi generates a cluster of keypoints around the number plate of the lead car, around the headrests visible through the rear screen and the edge of the vehicle.</td>
  </tr>
  <tr>
    <td class="tg-0pky">ORB</td>
    <td class="tg-0pky">1150</td>
    <td class="tg-0pky">6</td>
    <td class="tg-0pky">![ORB](/output/ORB/orb.gif) </td>
    <td class="tg-0pky">Keypoints are concentrated into a number of highly dense areas. The main concentrations appear to be around the lights. The keypoints are so concenrtated that the keypoint plots almost appear as concentric circles. The neighbourhood areas are the largest of the detectors. Orb returns a deifned number of keypoints each time which my default is set to 500. This was not modifed during this exerces. Orb was the 2nd fastest detector. ORB was one of the few detectors not to detect keypoints on the leading vehicles shadow.</td>
  </tr>
  <tr>
    <td class="tg-0pky">HARRIS</td>
    <td class="tg-0pky">248</td>
    <td class="tg-0pky">7</td>
    <td class="tg-0pky">![HARRIS](/output/HARRIS/harris.gif) </td>
    <td class="tg-0pky">Harris generates the fewest number of keypoints. Because it detects fewer keypoints there are less keypoints on the other vehicles as well. On the lead vehicle, there are keypoint clusters around the brake lights (both side and centre high mounted), but relatively few on the edge of the vehicle</td>
  </tr>
</tbody>
</table>













#### Findings

For automotive implementations, a good detector must be fast, but also be able to detect a large number of keypoints. Typically detectors are good at one or the other, which means that compromises must be made. In order to provide a truely objective proposal they should be evaluated against metrics for speed and performance.

Brisk returned the largest number of keypoints. Following Brisk, the order of most keypoints to fewest was, AKAZE, FAST, SIFT, SHITOMASI, ORB, with all providing a reasonable number of keypoints.  Harris detected very few keypoints, several magnitudes less than the others making it a selection match for the task.
