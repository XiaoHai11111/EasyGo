[out:json][timeout:180];

rel(5664101)->.district;
.district out tags bb;
.district map_to_area->.search_area;

(
  way(area.search_area)
    ["highway"~"^(motorway|motorway_link|trunk|trunk_link|primary|primary_link|secondary|secondary_link|tertiary|tertiary_link)$"];
  nwr(area.search_area)["amenity"="toilets"];
)->.features;

.features out tags center geom;
