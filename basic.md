---
layout: page
title: Basic usage
---

<ul>
{% for p in site.basic %}
  <li><a href="{{ site.baseurl }}{{ p.url }}">{{ p.title }}</a>
{% endfor %}
</ul>
