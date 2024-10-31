const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const { exec } = require('child_process');

const app = express();
const port = process.env.PORT || 5000;

app.use(cors());
app.use(bodyParser.json());

// API route to solve the linear program
app.post('/solve', (req, res) => {
  const inputData = req.body; // Example: problem data from the frontend

  // // Run the C solver program with child_process
  // exec('./solver', (error, stdout, stderr) => {
  //   if (error) {
  //     return res.status(500).json({ error: stderr });
  //   }
  //   // Send the output back to the frontend
  //   res.json({ result: stdout });
  // });
  console.log(inputData.constraints) ;
  res.json({result:inputData.constraints}) ;
});

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
});
