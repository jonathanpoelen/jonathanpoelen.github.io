// https://github.com/dwilliamson/donw.io/blob/master/public/js/github-comments.js

// use of ajax vs getJSON for headers use to get markdown (body vs body_htmml)
// todo: pages, configure issue url, open in new window?

let DoGithubComments = (function(){
  let createComment = function(comment)
  {
     let d = new Date(comment.created_at)
     let t = ''

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

     return t
  }

  const elist = document.getElementById("gh-comments-list")
  const ecomments = document.getElementById("gh-load-comments")

  let XHR = function(url, act) {
    let req = new XMLHttpRequest()
    req.onerror = function(event) {
      elist.innerHTML = '<p>Erreur de chargement. ' + btncomment + '</p>'
    }
    req.onload = function(event) {
      if (this.status === 200) {
        act(JSON.parse(event.currentTarget.response))
      }
      else {
        elist.innerHTML = "<p>Les commentaires ne sont pas encore ouverts.</p>"
        ecomments.style = 'display: none'
      }
    }
    req.open('get', url, true)
    req.setRequestHeader('accept', 'application/vnd.github.v3.html+json')
    req.send(null)
  }

  const per_page = 20
  const api_issue_url = "https://api.github.com/repos/jonathanpoelen/jonathanpoelen.github.io/issues/"
  const htlm_issue_url = "https://github.com/jonathanpoelen/jonathanpoelen.io/issues/"

  return function (issue_id)
  {
    const url = htlm_issue_url + issue_id
    const btncomment = "<a href='" + url + "#new_comment_field' rel='nofollow' class='btn'>Poster un commentaire sur Github</a>"

    XHR(api_issue_url + issue_id, function(json) {
      const baseurl = api_issue_url + issue_id + "/comments?per_page=" + per_page + "&page="
      const nb_comment = json.comments
      let page = 0

      if (json.user.id !== 1436727) {
        elist.innerHTML += createComment(json) + btncomment
      }

console.log(nb_comment)

      if (nb_comment > 1) {
        let openComments
        let extraMessage = ''
        openComments = function(){
          ++page
          XHR(baseurl + page, function(json){
            elist.innerHTML += json.reduce(function(s, comment){
              return s + createComment(comment)
            }, '') + btncomment + extraMessage
            ecomments.innerHTML = "Charger plus de commentaires (encore " + (nb_comment - ((page - 1) * per_page + json.length)) + ")"
          })

          if (page * per_page >= nb_comment) {
            ecomments.style = 'display: none'
            extraMessage = '<p>Fin des commentaires</p>'
          }
        }

        openComments()

        ecomments.onclick = function(e) {
          openComments()
          e.preventDefault()
        }
      }
      else {
        if (json.user.id === 1436727) {
          elist.innerHTML += "<div class='gh-comment'>Aucun commentaire pour le moment.</div>"
          ecomments.style = 'display: none'
        }
        elist.innerHTML += btncomment
      }
    })
  }
})()
