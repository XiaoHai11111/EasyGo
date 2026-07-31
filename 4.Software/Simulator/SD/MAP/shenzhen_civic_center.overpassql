[out:json][timeout:20];
(
  way["highway"~"^(primary|secondary|tertiary)$"](22.540,114.052,22.546,114.062);
  node["amenity"="toilets"](22.540,114.052,22.546,114.062);
);
out geom;
