// https://github.com/dwilliamson/donw.io/blob/master/public/js/github-comments.js

// use of ajax vs getJSON for headers use to get markdown (body vs body_htmml)
// todo: pages, configure issue url, open in new window?

var DoGithubComments = (function(){
  function ParseLinkHeader(link)
  {
    var entries = link.split(",")
    var links = {}
    for (var i in entries)
    {
      var entry = entries[i]
      var link = {}
      link.name = entry.match(/rel=\"([^\"]*)/)[1]
      link.url = entry.match(/<([^>]*)/)[1]
      link.page = entry.match(/page=(\d+).*$/)[1]
      links[link.name] = link
    }
    return links
  }

  var elist = document.getElementById("gh-comments-list")
  var ecomments = document.getElementById("gh-load-comments")
  var repo_name = "jonathanpoelen/jonathanpoelen.io"
  var api_url = "https://api.github.com/repos/" + repo_name

  return function (comment_id, page_id)
  {
    if (page_id === undefined)
      page_id = 1

    //var api_issue_url = api_url + "/issues/" + comment_id
    var api_comments_url = api_url + "/issues/" + comment_id + "/comments" + "?page=" + page_id
    var url = "https://github.com/" + repo_name + "/issues/" + comment_id
    var btncomment = "<a href='" + url + "#new_comment_field' rel='nofollow' class='btn'>Poster un commentaire sur Github</a>"

    var req = new XMLHttpRequest()
    req.onerror = function(event) {
      elist.innerHTML = '<p>Erreur de chargement. ' + btncomment + '</p>'
    }
    req.onload = function(event) {
      if (this.status === 200) {
        var json = JSON.parse(event.currentTarget.response)
        var t = btncomment
        var comment, d
        for (var i in json)
        {
          comment = json[i]
          d = new Date(comment.created_at)

          t += "<div class='gh-comment'>"
          t += "<div class='gh-comment-header'><img src='" + comment.user.avatar_url + "' width='24px'>"
          t += "<a class='gh-comment-user' href='" + comment.user.html_url + "'>" + comment.user.login + "</a>"
          t += " posté à <time datetime=" + d.getUTCFullYear()
          t += (d.getUTCMonth()<9?'-0':'-')+(d.getUTCMonth()+1)
          t += (d.getUTCDate()<=9?'-0':'-')+d.getUTCDate()
          t += (d.getUTCHours()<=9?' 0':' ')+d.getUTCHours()
          t += +(d.getUTCMinutes()<=9?' 0':' ')+d.getUTCMinutes()
          t += '">' + d.toUTCString() + "</time>"
          t += "</div><div class='gh-comment-content'>"
          t += comment.body_html
          t += "</div></div>"
        }

        t += btncomment

        elist.innerHTML += t

        // Setup comments button if there are more pages to display
        console.log(req)
        console.log(req.getResponseHeader("link"))
        var links = ParseLinkHeader(req.getResponseHeader('link') || '')
        if ("next" in links)
        {
          ecomments.onclick = function(e) {
            DoGithubComments(comment_id, page_id + 1)
            //return false
            e.preventDefault()
          }
          ecomments.style = 'display: visible'
        }
        else
        {
          ecomments.style = 'display: none'
        }
      } else {
        elist.innerHTML = "<p>Les commentaires ne sont pas encore ouverts.</p>"
        ecomments.style = 'display: none'
      }
    }
    req.open('get', api_comments_url, true)
    req.setRequestHeader('accept', 'application/vnd.github.v3.html+json')
    req.send(null)
  }
})()
