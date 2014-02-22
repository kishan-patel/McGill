--Q1
SELECT S.id \
FROM Series S, Games G \
WHERE S.id = G.id AND ABS(G.goals1-G.goals2) >= 4 \
GROUP BY S.id \
ORDER BY S.id ASC

--Q2
SELECT S.round \
FROM Series S, Games G \
WHERE S.id = G.id AND G.ot = 1 \
GROUP BY S.round 

--Q3
SELECT S1.id \ 
FROM Series S1 \
WHERE S1.id NOT IN ( \
  SELECT S2.id \ 
  FROM Series S2, Games G \ 
  WHERE S2.id = G.id AND G.ot = 1 \
  GROUP BY S2.id \  
)

--Q4
SELECT S.id \
FROM Series S \
WHERE S.id NOT IN ( \
  SELECT G1.id \
  FROM Games G1, Games G2 \
  WHERE G1.id = S.id AND  G2.id = S.id AND G1.game != G2.game AND (G1.goals1 != G2.goals1 OR G2.goals2 != G2.goals2) \
  GROUP BY G1.id \
) 

--Q5
SELECT S.id, G1.game, G1.goals1 + G1.goals2 as totalGoals \
FROM Series S, Games G1 \
WHERE G1.goals1 + G1.goals2 = ( \
  SELECT MIN(G2.goals1 + G2.goals2) \
  FROM Games G2 \ 
) AND S.id = G1.id

--Q6
SELECT S.team1, G.goals1, S.team2, g.goals2 \
FROM Series S, Games G \
WHERE S.id = G.id AND (S.team1 = 'Montreal Canadiens' OR S.team2 = 'Montreal Canadiens') \
ORDER BY S.id, G.game 

--Q7
SELECT S.team1, G.goals1, S.team2, g.goals2 \
FROM Series S, Games G \
WHERE S.id = G.id AND S.id = ( \
  SELECT max(S2.id) \
  FROM Series S2 \
  WHERE S2.team1 = 'Montreal Canadiens' OR S2.team2 = 'Montreal Canadiens' \
)

--Q8
SELECT AVG(G.goals1+G.goals2) as avgGoals \
FROM Games G

--Q9
SELECT S.id, COUNT(*) as Games \ 
FROM Series S, Games G \ 
WHERE S.id = G.id \
GROUP BY S.id

--Q10
Select S.id \
FROM Series S, Games G \ 
WHERE S.id = G.id \ 
GROUP BY S.id \
HAVING COUNT(*) < 7 

--Q11
SELECT S.id \
FROM Series S, Games G \
WHERE S.id = G.id AND S.team1 = 'Montreal Canadiens' or S.team2 = 'Montreal Canadiens' \
GROUP BY S.id \
HAVING COUNT(*) < 7

--Q12
SELECT S.id \
FROM Series S, Games G \
WHERE S.id = G.id AND G.ot = 1 \ 
GROUP BY S.id \
HAVING COUNT(*) >= 2

--Q13
SELECT S2.id \           
FROM  Series S2, Games G \
WHERE S2.id = G.id \
GROUP BY S2.id \
HAVING COUNT(*) = ( \ 
  SELECT MAX (g.gameCount) \
  FROM ( \
    SELECT COUNT(*) as gameCount \
    FROM Series S3, Games G2 \
    WHERE S3.id = G2.id \
    GROUP BY S3.id \
    ORDER BY gameCount DESC \
  ) AS g \
)

--Q14
SELECT * \
FROM ( \ 
  SELECT G.id, 'team 1' as team, count(*) as games_won \
  FROM Games G \
  WHERE G.goals1 > G.goals2 \
  Group by G.id \
  UNION \
  SELECT G.id, 'team 2' as team, count(*) as games_won \
  FROM Games G \
  WHERE G.goals2 > G.goals1 \
  Group By G.id \
  UNION \
  SELECT G.id, 'team 1' as team, 0 as games_won \
  FROM Games G \
  WHERE G.id NOT IN ( \
    SELECT G2.id \
    FROM Games G2 \
    WHERE G2.goals1 > G2.goals2 \
    GROUP BY G2.id \
  ) \
  GROUP By G.id \
  UNION \
  SELECT G.id, 'team 2' as team, 0 as games_won \
  FROM Games G \
  WHERE G.id NOT IN ( \
    SELECT G2.id \
    FROM Games G2 \
    WHERE G2.goals2 > G2.goals1 \
    GROUP By G2.id \
  ) \
) AS t \
ORDER BY t.id, team 

--Q15
CREATE TRIGGER UPDATE_GAMES_WON \
  AFTER INSERT ON Games \
  REFERENCING NEW AS NEW_GAME \
  FOR EACH ROW \
  BEGIN ATOMIC \
    UPDATE SERIES \
    SET \
      games_won1 = CASE WHEN NEW_GAME.goals1 > NEW_GAME.goals2 then games_won1 + 1 ELSE games_won1 END, \
      games_won2 = CASE WHEN NEW_GAME.goals2 > NEW_GAME.goals1 then games_won2 + 1 ELSE games_won2 END \ 
    WHERE id = NEW_GAME.id; \
  END
