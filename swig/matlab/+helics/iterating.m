function v = iterating()
  persistent vInitialized;
  if isempty(vInitialized)
    vInitialized = helicsMEX(0, 1329876569);
  end
  v = vInitialized;
end
